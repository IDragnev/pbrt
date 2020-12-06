#pragma once

#include "core/Primitive.hpp"

#include <vector>
#include <memory>

namespace idragnev::pbrt::accelerators {
    namespace bvh {
        struct BuildTree;
        enum class SplitMethod;
    } // namespace bvh

    class BVH : public Aggregate
    {
        struct LinearBVHNode
        {
        };

    public:
        BVH(std::vector<std::shared_ptr<const Primitive>> primitives,
            const bvh::SplitMethod m,
            const std::uint32_t maxPrimitivesInNode = 1);
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