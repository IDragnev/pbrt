#include "pbrt/reflection_models/SpecularTransmission.hpp"
#include "pbrt/core/reflection/Utility.hpp"
#include "pbrt/core/Material.hpp"

namespace idragnev::pbrt::reflection_models {
    using reflection::BxDFType;

    SpecularTransmission::SpecularTransmission(const Spectrum& T,
                                               Float etaA,
                                               Float etaB,
                                               TransportMode mode)
        : BxDF(BxDFType::TRANSMISSION | BxDFType::SPECULAR)
        , T(T)
        , etaA(etaA)
        , etaB(etaB)
        , fresnel(etaA, etaB)
        , mode(mode) {}

    Spectrum SpecularTransmission::sample_f(const Vector3f& wo,
                                            Vector3f& wi,
                                            const Point2f&,
                                            Float& pdf,
                                            Optional<BxDFType>) const {
        const bool entering = reflection::cosTheta(wo) > 0.f;
        const Float etaI = entering ? etaA : etaB;
        const Float etaT = entering ? etaB : etaA;
        const Float etaRatio = (etaT != 0.f) ? (etaI / etaT) : 1.f;

        Optional<Vector3f> optWi =
            reflection::refract(wo,
                                faceforward(Normal3f(0, 0, 1), wo),
                                etaRatio);

        if (optWi.has_value() == false) {
            return Spectrum();
        }

        wi = optWi.value();
        pdf = 1.f;

        const Float cosThetaI = reflection::cosTheta(wi);
        Spectrum ft = T * (Spectrum(1.f) - fresnel.eval(cosThetaI));

        // account for non-symmetry with transmission to different medium
        if (mode == TransportMode::RADIANCE) {
            ft *= (etaI * etaI) / (etaT * etaT);
        }

        return ft / Spectrum(reflection::absCosTheta(wi));
    }
} // namespace idragnev::pbrt::reflection_models