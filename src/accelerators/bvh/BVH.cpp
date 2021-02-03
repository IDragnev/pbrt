#include "pbrt/accelerators/bvh/BVH.hpp"
#include "pbrt/accelerators/bvh/RecursiveBuilder.hpp"
#include "pbrt/accelerators/bvh/HLBVHBuilder.hpp"
#include "pbrt/core/SurfaceInteraction.hpp"
#include "pbrt/memory/Memory.hpp"

namespace idragnev::pbrt::accelerators {
    class NodeIndicesStack
    {
    public:
        void push(const std::size_t index) { data[top++] = index; }
        std::size_t pop() { return data[--top]; }

        bool isEmpty() const noexcept { return top == 0; }

    private:
        std::size_t top = 0;
        std::size_t data[64] = {};
    };

    struct BVH::FlattenResult
    {
        std::size_t rootIndex = 0;
        std::size_t linearNodesWritten = 0;
    };

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4324) // structure padding due to alignment specifier
#endif
    // Ensure that nodes do not straddle cache lines
    struct alignas(32) BVH::LinearBVHNode
    {
        static LinearBVHNode Leaf(const std::size_t firstPrimitiveIndex,
                                  const std::uint16_t primitivesCount,
                                  const Bounds3f& bounds) {
            return LinearBVHNode{
                .bounds = bounds,
                .firstPrimitiveIndex = firstPrimitiveIndex,
                .primitivesCount = primitivesCount,
            };
        }

        static LinearBVHNode Interior(const std::size_t secondChildIndex,
                                      const std::uint8_t splitAxis,
                                      const Bounds3f& bounds) {
            return LinearBVHNode{
                .bounds = bounds,
                .secondChildIndex = secondChildIndex,
                .splitAxis = splitAxis,
            };
        }

        bool isLeaf() const noexcept { return primitivesCount > 0; }

        Bounds3f bounds;
        union
        {
            std::size_t firstPrimitiveIndex;
            std::size_t secondChildIndex;
        };
        std::uint16_t primitivesCount = 0;
        std::uint8_t splitAxis = 0;
    };
#ifdef _MSC_VER
    #pragma warning(pop)
#endif

    BVH::BVH(std::vector<std::shared_ptr<const Primitive>> prims,
             const bvh::SplitMethod splitMethod,
             const std::uint32_t maxPrimitivesInNode)
        : maxPrimitivesInNode(std::min(maxPrimitivesInNode, 255u))
        , primitives(std::move(prims)) {
        if (this->primitives.empty() == false) {
            memory::MemoryArena arena{1024 * 1024};

            const bvh::BuildTree tree = buildBVHTree(splitMethod, arena);
            this->nodes =
                memory::allocCacheAligned<LinearBVHNode>(tree.nodesCount);
            [[maybe_unused]] const auto result = flattenBVHTree(*tree.root, 0);

            assert(result.linearNodesWritten == tree.nodesCount);
        }
    }

    bvh::BuildTree BVH::buildBVHTree(const bvh::SplitMethod splitMethod,
                                     memory::MemoryArena& arena) {
        const auto recursiveBuilder =
            bvh::RecursiveBuilder{splitMethod, this->maxPrimitivesInNode};
        auto hlbvhBuilder = bvh::HLBVHBuilder{this->maxPrimitivesInNode};

        bvh::BuildResult result =
            splitMethod == bvh::SplitMethod::HLBVH
                ? hlbvhBuilder(arena, this->primitives)
                : recursiveBuilder(arena, this->primitives);

        this->primitives = std::move(result.orderedPrimitives);

        return result.tree;
    }

    BVH::FlattenResult BVH::flattenBVHTree(const bvh::BuildNode& buildNode,
                                           const std::size_t linearNodeIndex) {
        LinearBVHNode& node = this->nodes[linearNodeIndex];

        if (const auto isLeafNode = buildNode.primitivesCount > 0; isLeafNode) {
            assert(buildNode.primitivesCount <= 65536);

            node = LinearBVHNode::Leaf(
                buildNode.firstPrimitiveIndex,
                static_cast<std::uint16_t>(buildNode.primitivesCount),
                buildNode.bounds);

            return FlattenResult{
                .rootIndex = linearNodeIndex,
                .linearNodesWritten = 1,
            };
        }
        else {
            const auto left =
                flattenBVHTree(*buildNode.children[0], linearNodeIndex + 1);
            const auto right =
                flattenBVHTree(*buildNode.children[1],
                               left.rootIndex + left.linearNodesWritten);

            node = LinearBVHNode::Interior(
                right.rootIndex,
                static_cast<std::uint8_t>(buildNode.splitAxis),
                buildNode.bounds);

            return FlattenResult{
                .rootIndex = linearNodeIndex,
                .linearNodesWritten =
                    left.linearNodesWritten + right.linearNodesWritten + 1,
            };
        }
    }

    BVH::~BVH() { memory::freeAligned(nodes); }

    Bounds3f BVH::worldBound() const {
        return (this->nodes != nullptr) ? nodes[0].bounds : Bounds3f{};
    }

    Optional<SurfaceInteraction> BVH::intersect(const Ray& ray) const {
        Optional<SurfaceInteraction> result = pbrt::nullopt;

        traverseIntersect(
            [this, &result](const LinearBVHNode& leafNode, const Ray& ray) {
                result = intersectLeafNodePrims(leafNode, ray);
                return false;
            },
            ray);

        return result;
    }

    bool BVH::intersectP(const Ray& ray) const {
        bool result = false;

        traverseIntersect(
            [this, &result](const LinearBVHNode& leafNode, const Ray& ray) {
                result = intersectPLeafNodePrims(leafNode, ray);
                return result;
            },
            ray);

        return result;
    }

    // Traverses the tree, ignoring subtrees which are not intersected by `ray`.
    // For intersected internal nodes, visits the two child trees in
    // a front-to-back order.
    // Calls `intersectLeaf` with `ray` for each intersected leaf node.
    // (!) Stops the traversal if `intersectLeaf` returns true -
    // `intersectLeaf` indicates whether the traversal should stop. (!)
    void BVH::traverseIntersect(
        std::function<bool(const LinearBVHNode&, const Ray&)> intersectLeaf,
        const Ray& ray) const {
        if (this->nodes == nullptr) {
            return;
        }

        const Vector3f invDir{1.f / ray.d.x, 1.f / ray.d.y, 1.f / ray.d.z};
        const std::size_t dirIsNegative[3] = {invDir.x < 0.f ? 1u : 0u,
                                              invDir.y < 0.f ? 1u : 0u,
                                              invDir.z < 0.f ? 1u : 0u};

        NodeIndicesStack nodesToVisit;
        nodesToVisit.push(0);

        while (nodesToVisit.isEmpty() == false) {
            const std::size_t currentNodeIndex = nodesToVisit.pop();
            const LinearBVHNode& node = this->nodes[currentNodeIndex];

            if (node.bounds.intersectP(ray, invDir, dirIsNegative)) {
                if (node.isLeaf()) {
                    if (bool stop = intersectLeaf(node, ray); stop) {
                        return;
                    }
                }
                else {
                    const std::size_t leftChildIndex = currentNodeIndex + 1;

                    if (dirIsNegative[node.splitAxis] == 1) {
                        nodesToVisit.push(leftChildIndex);
                        nodesToVisit.push(node.secondChildIndex);
                    }
                    else {
                        nodesToVisit.push(node.secondChildIndex);
                        nodesToVisit.push(leftChildIndex);
                    }
                }
            }
        }
    }

    Optional<SurfaceInteraction>
    BVH::intersectLeafNodePrims(const LinearBVHNode& node,
                                const Ray& ray) const {
        Optional<SurfaceInteraction> result = pbrt::nullopt;

        if (node.isLeaf()) {
            const auto primsRange =
                std::span<const std::shared_ptr<const Primitive>>{
                    this->primitives.cbegin() + node.firstPrimitiveIndex,
                    node.primitivesCount};

            for (const auto& primitive : primsRange) {
                result = primitive->intersect(ray);
            }
        }

        return result;
    }

    bool BVH::intersectPLeafNodePrims(const LinearBVHNode& node,
                                 const Ray& ray) const {
        if (node.isLeaf() == false) {
            return false;
        }

        const auto primsRange =
            std::span<const std::shared_ptr<const Primitive>>{
                this->primitives.cbegin() + node.firstPrimitiveIndex,
                node.primitivesCount};

        return std::any_of(
            primsRange.begin(),
            primsRange.end(),
            [&ray](const auto& p) { return p->intersectP(ray); });
    }
} // namespace idragnev::pbrt::accelerators