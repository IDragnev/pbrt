#pragma once

#include "core.hpp"
#include "Point3.hpp"
#include "Vector3.hpp"

namespace idragnev::pbrt {
    class Ray
    {
    public:
        Ray() = default;
        Ray(const Point3f& o,
            const Vector3f& d,
            const Float tMax = constants::Infinity,
            const Float time = 0.f,
            const Medium* medium = nullptr);

        Point3f operator()(const Float t) const { return o + t * d; }

        bool hasNaNs() const {
            return (o.hasNaNs() || d.hasNaNs() || isNaN(tMax));
        }

        Point3f o;
        Vector3f d;
        mutable Float tMax = constants::Infinity;
        Float time = 0.0f;
        const Medium* medium = nullptr;
    };
} // namespace idragnev::pbrt