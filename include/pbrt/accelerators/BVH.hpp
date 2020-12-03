#pragma once

#include "core/Primitive.hpp"
#include "BVHBuilders.hpp"

namespace idragnev::pbrt::accelerators {
    class BVH : public Aggregate
    {
        struct LinearBVHNode
        {
        };

    public:
        BVH(std::vector<std::shared_ptr<const Primitive>> primitives,
            const std::uint32_t maxPrimitivesInNode = 1,
            const bvh::SplitMethod m = bvh::SplitMethod::SAH);
        ~BVH();

        Bounds3f worldBound() const override;

        std::optional<SurfaceInteraction>
        intersect(const Ray& ray) const override;
        bool intersectP(const Ray& ray) const override;

    private:
        bvh::BuildTree buildBVHTree(const bvh::SplitMethod m);

    private:
        std::uint32_t maxPrimitivesInNode = 1;
        std::vector<std::shared_ptr<const Primitive>> primitives;
        LinearBVHNode* nodes = nullptr;
    };
} // namespace idragnev::pbrt::accelerators