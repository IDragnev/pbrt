#include "pbrt/core/reflection/BxDF.hpp"
#include "pbrt/core/reflection/Utility.hpp"

namespace idragnev::pbrt::reflection {
    Spectrum FresnelConductor::eval(Float cosThetaI) const {
        return fresnelReflConductor(std::abs(cosThetaI), etaI, etaT, k);
    }

    Spectrum FresnelDielectric::eval(Float cosThetaI) const {
        const Float v = fresnelReflDielectric(cosThetaI, etaI, etaT);

        return Spectrum(v);
    }

    Spectrum FresnelNoOp::eval(Float) const { return Spectrum(1.f); }
} // namespace idragnev::pbrt::reflection