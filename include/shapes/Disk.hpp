#pragma once

#include "core/core.hpp"
#include "core/Shape.hpp"

namespace idragnev::pbrt {
    class Disk : public Shape
    {
    public:
        Disk(const Transformation& objectToWorld,
             const Transformation& worldToObject,
             const bool reverseOrientation,
             const Float height,
             const Float radius,
             const Float innerRadius,
             const Float phiMax) noexcept;

        Bounds3f objectBound() const override;

        std::optional<HitRecord>
        intersect(const Ray& ray, const bool testAlphaTexture) const override;

        bool intersectP(const Ray& ray,
                        const bool testAlphaTexture) const override;

        Float area() const override;

    private:
        template <typename R, typename S, typename F>
        R intersectImpl(const Ray& ray, F failure, S success) const;

        static Float computePhi(const Point3f& hitPoint);

        HitRecord makeHitRecord(const Ray& ray,
                                const Point3f& hitPoint,
                                const EFloat& t,
                                const Float phi,
                                const Float distToCenterSquared) const;

    private:
        Float height;
        Float radius;
        Float innerRadius;
        Float phiMax;
    };
} // namespace idragnev::pbrt