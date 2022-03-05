#include "pbrt/reflection_models/LambertianTransmission.hpp"

namespace idragnev::pbrt::reflection_models {
    using reflection::BxDFType;

    LambertianTransmission::LambertianTransmission(const Spectrum& T)
        : BxDF(BxDFType(BxDFType::TRANSMISSION | BxDFType::DIFFUSE))
        , T(T) {}

    Spectrum LambertianTransmission::f(const Vector3f&, const Vector3f&) const {
        return T * math::constants::InvPi;
    }

    Spectrum LambertianTransmission::rho(const Vector3f&,
                                       const std::span<const Point2f>) const {
        return T;
    }
    Spectrum LambertianTransmission::rho(const std::span<const Point2f>,
                                       const std::span<const Point2f>) const {
        return T;
    }
} // namespace idragnev::pbrt::reflection_models