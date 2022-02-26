#pragma once

#include "pbrt/core/reflection/BxDF.hpp"

namespace idragnev::pbrt::reflection_models {
    class SpecularReflection : public reflection::BxDF
    {
    public:
        SpecularReflection(const Spectrum& R,
                           const reflection::Fresnel* fresnel);

        Spectrum f(const Vector3f&, const Vector3f&) const override {
            return Spectrum(0.f);
        }
        Spectrum
        sample_f(const Vector3f& wo,
                 Vector3f& wi,
                 const Point2f& sample,
                 Float& pdf,
                 Optional<reflection::BxDFType> sampledType) const override;

        // Float pdf(const Vector3f& wo, const Vector3f& wi) const override {
        //     return 0.f;
        // }
        // std::string toString() const override;

    private:
        Spectrum R;
        const reflection::Fresnel* fresnel = nullptr;
    };
} // namespace idragnev::pbrt::reflection_models