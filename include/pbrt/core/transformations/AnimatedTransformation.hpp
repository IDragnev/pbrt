#pragma once

#include "pbrt/core/core.hpp"
#include "TRS.hpp"

namespace idragnev::pbrt {
    class AnimatedTransformation
    {
    private:
        struct DerivativeTerm
        {
            Float operator()(const Point3f& p) const noexcept;

            Float kc = 0.f;
            Float kx = 0.f;
            Float ky = 0.f;
            Float kz = 0.f;
        };

        struct Coefficients
        {
            Float c1 = 0.f;
            Float c2 = 0.f;
            Float c3 = 0.f;
            Float c4 = 0.f;
            Float c5 = 0.f;
        };

    public:
        AnimatedTransformation(const Transformation& startTransform,
                               const Float startTime,
                               const Transformation& endTransform,
                               const Float endTime);

        Transformation interpolate(const Float time) const;

        Bounds3f motionBounds(const Bounds3f& b) const;

        Ray operator()(const Ray& r) const;
        RayDifferential operator()(const RayDifferential& r) const;
        Point3f operator()(const Float time, const Point3f& p) const;
        Vector3f operator()(const Float time, const Vector3f& v) const;

        bool hasScale() const noexcept;

    private:
        template <typename T>
        auto transform(const Float time, const T& x) const;

        Bounds3f pointMotionBounds(const Point3f& p) const;

        static void intervalFindZeros(const Coefficients& cs,
                                      const Float theta,
                                      const Intervalf& tInterval,
                                      Float zeros[8],
                                      unsigned& zerosCount,
                                      int depth = 8);

    private:
        const Transformation* startTransform = nullptr;
        const Transformation* endTransform = nullptr;
        Float startTime = 0.f;
        Float endTime = 0.f;
        TRS startTRS;
        TRS endTRS;
        bool actuallyAnimated = false;
        bool hasRotation = false;
        DerivativeTerm c1[3];
        DerivativeTerm c2[3];
        DerivativeTerm c3[3];
        DerivativeTerm c4[3];
        DerivativeTerm c5[3];
    };
} // namespace idragnev::pbrt
