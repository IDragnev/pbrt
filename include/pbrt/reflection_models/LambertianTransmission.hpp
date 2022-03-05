#pragma once

#include "pbrt/core/reflection/BxDF.hpp"

namespace idragnev::pbrt::reflection_models {
    class LambertianTransmission : public reflection::BxDF
    {
    public:
        LambertianTransmission(const Spectrum& T);

        Spectrum f(const Vector3f& wo, const Vector3f& wi) const override;
        Spectrum rho(const Vector3f&,
                     const std::span<const Point2f> samples) const override;
        Spectrum rho(const std::span<const Point2f> samples1,
                     const std::span<const Point2f> samples2) const override;

        // std::string toString() const override;

    private:
        Spectrum T;
    };
} // namespace idragnev::pbrt::reflection_models