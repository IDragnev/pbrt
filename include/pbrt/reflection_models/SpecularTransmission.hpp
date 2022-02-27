#pragma once

#include "pbrt/core/reflection/BxDF.hpp"

namespace idragnev::pbrt::reflection_models {
    class SpecularTransmission : public reflection::BxDF
    {
    public:
        SpecularTransmission(const Spectrum& T,
                             Float etaA,
                             Float etaB,
                             TransportMode mode);

        Spectrum f(const Vector3f&, const Vector3f&) const override {
            return Spectrum(0.f);
        }
        Spectrum
        sample_f(const Vector3f& wo,
                 Vector3f& wi,
                 const Point2f& sample,
                 Float& pdf,
                 Optional<reflection::BxDFType> sampledType) const override;

        //Float pdf(const Vector3f& wo, const Vector3f& wi) const override { return 0; }
        //std::string toString() const override;

    private:
        Spectrum T;
        Float etaA = 1.f;
        Float etaB = 1.f;
        reflection::FresnelDielectric fresnel;
        TransportMode mode;
    };
} // namespace idragnev::pbrt::reflection_models