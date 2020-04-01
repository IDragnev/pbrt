#include "AnimatedTransformation.hpp"
#include "Transformation.hpp"
#include "TRS.hpp"
#include "Ray.hpp"
#include "RayDifferential.hpp"

namespace idragnev::pbrt {
    AnimatedTransformation::AnimatedTransformation(
        const Transformation& startTransform, const Float startTime,
        const Transformation& endTransform, const Float endTime)
        : startTransform{&startTransform}
        , endTransform{&endTransform}
        , startTime{startTime}
        , endTime{endTime}
        , actuallyAnimated{startTransform != endTransform}
    {
        if (!actuallyAnimated) {
            return;
        }

        startTRS = decompose(startTransform.matrix());
        endTRS = decompose(endTransform.matrix());

        const auto rotationsDot = dot(startTRS.R, endTRS.R);
        if (const auto isAngleObtuse = rotationsDot < 0.f;
            isAngleObtuse) {
            endTRS.R = -endTRS.R;
        }
        hasRotation = rotationsDot < 0.9995f;
    }

    Transformation AnimatedTransformation::interpolate(const Float time) const {
        if (!actuallyAnimated || time <= startTime) {
            return *startTransform;
        }
        else if (time >= endTime) {
            return *endTransform;
        }

        const auto dt = (time - startTime) / (endTime - startTime);
        const auto T = lerp(dt, startTRS.T, endTRS.T);
        const auto R = slerp(dt, startTRS.R, endTRS.R);

        auto S = Matrix4x4{};
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; j < 3; ++j) {
                S.m[i][j] = lerp(dt, startTRS.S.m[i][j], endTRS.S.m[i][j]);
            }
        }

        return translation(T) * R.toTransformation() * Transformation{S};
    }

    Ray AnimatedTransformation::operator()(const Ray& ray) const {
        return transform(ray.time, ray);
    }

    RayDifferential AnimatedTransformation::operator()(const RayDifferential& ray) const {
        return transform(ray.time, ray);
    }

    Point3f AnimatedTransformation::operator()(const Float time, const Point3f& p) const {
        return transform(time, p);
    }

    Vector3f AnimatedTransformation::operator()(const Float time, const Vector3f& v) const {
        return transform(time, v);
    }
}