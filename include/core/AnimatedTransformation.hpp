#pragma once

#include "core.hpp"
#include "TRS.hpp"

namespace idragnev::pbrt {
    class AnimatedTransformation
    {
    public:
        AnimatedTransformation(
            const Transformation& startTransform, const Float startTime,
            const Transformation& endTransform, const Float endTime);

        Transformation interpolate(const Float time) const;

        Ray operator()(const Ray& r) const;
        RayDifferential operator()(const RayDifferential& r) const;
        Point3f operator()(const Float time, const Point3f& p) const;
        Vector3f operator()(const Float time, const Vector3f& v) const;

    private:
        template <typename T>
        auto transform(const Float time, const T& x) const;

    private:
        const Transformation* startTransform = nullptr;
        const Transformation* endTransform = nullptr;
        Float startTime = 0.f;
        Float endTime = 0.f;
        TRS startTRS;
        TRS endTRS;
        bool actuallyAnimated = false;
        bool hasRotation = false;
    };

    template <typename T>
    auto AnimatedTransformation::transform(const Float time, const T& x) const {
        if (!actuallyAnimated || time <= startTime) {
            return (*startTransform)(x);
        }
        else if (time >= endTime) {
            return (*endTransform)(x);
        }
        else {
            const auto transformation = this->interpolate(time);
            return transformation(x);
        }
    }
} //namespace idragnev::pbrt 
