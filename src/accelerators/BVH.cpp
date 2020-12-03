#include "accelerators/BVH.hpp"
#include "core/SurfaceInteraction.hpp"
#include "memory/Memory.hpp"

namespace idragnev::pbrt::accelerators {
    BVH::BVH(std::vector<std::shared_ptr<const Primitive>> prims,
             const std::uint32_t maxPrimitivesInNode,
             const bvh::SplitMethod splitMethod)
        : maxPrimitivesInNode(std::min(maxPrimitivesInNode, 255u))
        , primitives(std::move(prims)) {
        if (this->primitives.empty() == false) {
            const bvh::BuildTree tree = buildBVHTree(splitMethod);
            this->nodes =
                memory::allocCacheAligned<BVH::LinearBVHNode>(tree.nodesCount);
            // flattenBVHTree(tree.root);
        }
    }

    bvh::BuildTree BVH::buildBVHTree(const bvh::SplitMethod splitMethod) {
        memory::MemoryArena arena{1024 * 1024};

        auto recursiveBuilder = bvh::RecursiveBuilder{splitMethod};
        bvh::BuildResult result =
            splitMethod == bvh::SplitMethod::HLBVH
                ? bvh::BuildResult{}
                : recursiveBuilder(arena, this->primitives);

        this->primitives = std::move(result.orderedPrimitives);

        return result.tree;
    }

    BVH::~BVH() { memory::freeAligned(nodes); }

    Bounds3f BVH::worldBound() const { return Bounds3f{}; }

    std::optional<SurfaceInteraction> BVH::intersect(const Ray&) const {
        return std::nullopt;
    }

    bool BVH::intersectP(const Ray&) const { return false; }
} // namespace idragnev::pbrt::accelerators