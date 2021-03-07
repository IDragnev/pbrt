#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/math/Point3.hpp"
#include "pbrt/core/math/Vector3.hpp"

namespace idragnev::pbrt {
    class Ray
    {
    public:
        Ray() = default;
        Ray(const Point3f& o,
            const Vector3f& d,
            const Float tMax = constants::Infinity,
            const Float time = 0.f,
            const Medium* medium = nullptr)
            : o(o)
            , d(d)
            , tMax(tMax)
            , time(time)
            , medium(medium) {}

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