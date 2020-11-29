#pragma once

#include "core/Primitive.hpp"

#include <vector>
#include <memory>

namespace idragnev::pbrt::accelerators {
    namespace bvh {
        struct BuildNode;
        struct PrimitiveInfo;
        struct MortonPrimitive;
        struct LinearBVHNode;

        enum class SplitMethod
        {
            SAH,
            HLBVH,
            middle,
            equalCounts,
        };
    } // namespace bvh

    class BVH : public Aggregate
    {
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
        std::uint32_t maxPrimitivesInNode = 1;
        bvh::SplitMethod splitMethod = bvh::SplitMethod::SAH;
        std::vector<std::shared_ptr<const Primitive>> primitives;
        bvh::LinearBVHNode* nodes = nullptr;
    };
} // namespace idragnev::pbrt::accelerators