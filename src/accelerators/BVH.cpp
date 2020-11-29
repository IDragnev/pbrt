#include "accelerators/BVH.hpp"
#include "core/Bounds3.hpp"
#include "core/SurfaceInteraction.hpp"

namespace idragnev::pbrt::accelerators {
    BVH::BVH(std::vector<std::shared_ptr<const Primitive>> primitives,
             const std::uint32_t maxPrimitivesInNode,
             const bvh::SplitMethod splitMethod)
        : maxPrimitivesInNode(std::min(maxPrimitivesInNode, 255u))
        , splitMethod(splitMethod)
        , primitives(std::move(primitives)) {
        if (primitives.empty()) {
            return;
        }

        // build BVH
    }

    BVH::~BVH() { memory::freeAligned(nodes); }

    Bounds3f BVH::worldBound() const { return Bounds3f{}; }

    std::optional<SurfaceInteraction> BVH::intersect(const Ray&) const {
        return std::nullopt;
    }

    bool BVH::intersectP(const Ray&) const { return false; }
} // namespace idragnev::pbrt::accelerators