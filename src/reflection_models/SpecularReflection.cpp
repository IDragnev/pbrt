#include "pbrt/reflection_models/SpecularReflection.hpp"
#include "pbrt/core/reflection/Utility.hpp"

namespace idragnev::pbrt::reflection_models {
    using reflection::BxDFType;

    SpecularReflection::SpecularReflection(const Spectrum& R,
                                           const reflection::Fresnel* fresnel)
        : BxDF(BxDFType::REFLECTION | BxDFType::SPECULAR)
        , R(R)
        , fresnel(fresnel) {}

    Spectrum SpecularReflection::sample_f(const Vector3f& wo,
                                          Vector3f& wi,
                                          const Point2f&,
                                          Float& pdf,
                                          Optional<BxDFType>) const {
        pdf = 1.f;
        // perfect specular reflection direction
        // wi = -wo + 2 * dot(wo, n) * n; simplified because
        // n = (0, 0, 1) in BRDF coordinate system
        wi = Vector3f(-wo.x, -wo.y, wo.z);

        const auto absCosTheta = Spectrum(reflection::absCosTheta(wi));

        return fresnel->eval(reflection::cosTheta(wi)) * R / absCosTheta;
    }
} // namespace idragnev::pbrt::reflection_models