#pragma once

#include "core/core.hpp"
#include "core/Shape.hpp"

namespace idragnev::pbrt {
    class Cylinder : public Shape
    {
    public:
        Cylinder(const Transformation& objectToWorld,
                 const Transformation& worldToObject,
                 const bool reverseOrientation,
                 const Float radius,
                 const Float zMin,
                 const Float zMax,
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

        HitRecord makeHitRecord(const Ray& ray,
                                const Point3f& hitPoint,
                                const EFloat& t,
                                const Float phi) const;

        std::optional<QuadraticRoots>
        findIntersectionParams(const Ray& ray,
                               const Vector3f& oErr,
                               const Vector3f& dErr) const;

        Point3f computeHitPoint(const Ray& ray, const EFloat& t) const;

        static Float computePhi(const Point3f& hitPoint);

    private:
        Float radius;
        Float zMin;
        Float zMax;
        Float phiMax;
    };
} // namespace idragnev::pbrt