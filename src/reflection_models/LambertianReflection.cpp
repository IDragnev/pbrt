#include "pbrt/reflection_models/LambertianReflection.hpp"

namespace idragnev::pbrt::reflection_models {
    using reflection::BxDFType;

    LambertianReflection::LambertianReflection(const Spectrum& R)
        : BxDF(BxDFType(BxDFType::REFLECTION | BxDFType::DIFFUSE))
        , R(R) {}

    Spectrum LambertianReflection::f(const Vector3f&, const Vector3f&) const {
        return R * math::constants::InvPi;
    }

    Spectrum LambertianReflection::rho(const Vector3f&,
                                       const std::span<const Point2f>) const {
        return R;
    }
    Spectrum LambertianReflection::rho(const std::span<const Point2f>,
                                       const std::span<const Point2f>) const {
        return R;
    }
} // namespace idragnev::pbrt::reflection_models