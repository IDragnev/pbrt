#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/math/Vector3.hpp"
#include "pbrt/core/color/Spectrum.hpp"

namespace idragnev::pbrt::reflection {
    inline Float cosTheta(const Vector3f& w) { return w.z; }
    inline Float cos2Theta(const Vector3f& w) { return w.z * w.z; }
    inline Float absCosTheta(const Vector3f& w) { return std::abs(w.z); }

    inline Float sin2Theta(const Vector3f& w) {
        return std::max(Float(0), Float(1) - cos2Theta(w));
    }
    inline Float sinTheta(const Vector3f& w) { return std::sqrt(sin2Theta(w)); }

    inline Float tanTheta(const Vector3f& w) {
        return sinTheta(w) / cosTheta(w);
    }
    inline Float tan2Theta(const Vector3f& w) {
        return sin2Theta(w) / cos2Theta(w);
    }

    inline Float cosPhi(const Vector3f& w) {
        const Float sinTht = sinTheta(w);
        return (sinTht == 0.f) ? 1 : clamp(w.x / sinTht, Float(-1), Float(1));
    }
    inline Float sinPhi(const Vector3f& w) {
        const Float sinTht = sinTheta(w);
        return (sinTht == 0.f) ? 0.f : clamp(w.y / sinTht, Float(-1), Float(1));
    }

    inline Float cos2Phi(const Vector3f& w) { return cosPhi(w) * cosPhi(w); }
    inline Float sin2Phi(const Vector3f& w) { return sinPhi(w) * sinPhi(w); }

    Float cosDPhi(const Vector3f& wa, const Vector3f& wb);

    Float fresnelReflDielectric(Float cosThetaI, Float etaI, Float etaT);
    Spectrum fresnelReflConductor(Float cosThetaI,
                                  const Spectrum& etaI,
                                  const Spectrum& etaT,
                                  const Spectrum& k);
} // namespace idragnev::pbrt::reflection