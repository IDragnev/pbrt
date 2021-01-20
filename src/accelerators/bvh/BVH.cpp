#include "pbrt/accelerators/bvh/BVH.hpp"
#include "pbrt/accelerators/bvh/RecursiveBuilder.hpp"
#include "pbrt/accelerators/bvh/HLBVHBuilder.hpp"
#include "pbrt/core/SurfaceInteraction.hpp"
#include "pbrt/memory/Memory.hpp"

namespace idragnev::pbrt::accelerators {

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

        static LinearBVHNode Interior(const std::size_t secondChildOffset,
                                      const std::uint8_t splitAxis,
                                      const Bounds3f& bounds) {
            return LinearBVHNode{
                .bounds = bounds,
                .secondChildOffset = secondChildOffset,
                .splitAxis = splitAxis,
            };
        }

        Bounds3f bounds;
        union
        {
            std::size_t firstPrimitiveIndex;
            std::size_t secondChildOffset;
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
            const auto result = flattenBVHTree(*tree.root, 0);

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

    Bounds3f BVH::worldBound() const { return Bounds3f{}; }

    Optional<SurfaceInteraction> BVH::intersect(const Ray&) const {
        return pbrt::nullopt;
    }

    bool BVH::intersectP(const Ray&) const { return false; }
} // namespace idragnev::pbrt::accelerators