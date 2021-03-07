#pragma once

#include "pbrt/core/math/Math.hpp"
#include "pbrt/core/math/Vector3.hpp"

namespace idragnev::pbrt {
    inline Vector3f sphericalDirection(const Float sinTheta,
                                       const Float cosTheta,
                                       const Float phi) {
        return Vector3f(sinTheta * std::cos(phi),
                        sinTheta * std::sin(phi),
                        cosTheta);
    }

    inline Vector3f sphericalDirection(const Float sinTheta,
                                       const Float cosTheta,
                                       const Float phi,
                                       const Basis3f& basis) {
        return sinTheta * std::cos(phi) * basis.u +
               sinTheta * std::sin(phi) * basis.v +
               cosTheta * basis.w;
    }

    inline Float sphericalTheta(const Vector3f& v) {
        return std::acos(clamp(v.z, -1.f, 1.f));
    }

    inline Float sphericalPhi(const Vector3f& v) {
        const Float p = std::atan2(v.y, v.x);
        return (p < 0.f) ? (p + 2.f * math::constants::Pi) : p;
    }
} // namespace idragnev::pbrt
