#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/math/Vector3.hpp"
#include "pbrt/core/color/Spectrum.hpp"

#include <span>
#include <string>

namespace idragnev::pbrt::reflection {
    enum class BxDFType
    {
        // clang-format off
        REFLECTION   = 1 << 0,
        TRANSMISSION = 1 << 1,
        DIFFUSE      = 1 << 2,
        GLOSSY       = 1 << 3,
        SPECULAR     = 1 << 4,
        ALL          = REFLECTION | TRANSMISSION | DIFFUSE | GLOSSY | SPECULAR,
        // clang-format on
    };

    class BxDF
    {
    public:
        BxDF(const BxDFType type) : type(type) {}
        virtual ~BxDF() = default;

        bool matchesFlags(const BxDFType flags) const {
            using T = std::underlying_type_t<BxDFType>;
            const T tt = static_cast<T>(this->type);

            return (tt & static_cast<T>(flags)) == tt;
        }

        virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const = 0;
        virtual Spectrum sample_f(const Vector3f& wo,
                                  Vector3f& wi,
                                  const Point2f& sample,
                                  Float& pdf,
                                  BxDFType* sampledType = nullptr) const;

        virtual Spectrum
        rho(const Vector3f& wo, const std::span<const Point2f> samples) const;
        virtual Spectrum rho(const std::span<const Point2f> samples1,
                             const std::span<const Point2f> samples2) const;

        //virtual Float pdf(const Vector3f& wo, const Vector3f& wi) const;
        //virtual std::string toString() const = 0;

        const BxDFType type;
    };

    class ScaledBxDF : public BxDF
    {
    public:
        ScaledBxDF(const BxDF* bxdf, const Spectrum& scale)
            : BxDF(bxdf->type)
            , bxdf(bxdf)
            , scale(scale) {}

        Spectrum f(const Vector3f& wo, const Vector3f& wi) const override {
            return scale * bxdf->f(wo, wi);
        }
        Spectrum sample_f(const Vector3f& wo,
                          Vector3f& wi,
                          const Point2f& sample,
                          Float& pdf,
                          BxDFType* sampledType) const override {
            Spectrum f = bxdf->sample_f(wo, wi, sample, pdf, sampledType);
            return scale * f;
        }

        Spectrum rho(const Vector3f& wo,
                     const std::span<const Point2f> samples) const override {
            return scale * bxdf->rho(wo, samples);
        }
        Spectrum rho(const std::span<const Point2f> samples1,
                     const std::span<const Point2f> samples2) const override {
            return scale * bxdf->rho(samples1, samples2);
        }

    private:
        const BxDF* bxdf;
        Spectrum scale;
    };

    class Fresnel
    {
    public:
        virtual ~Fresnel() = default;

        virtual Spectrum eval(Float cosI) const = 0;
        //virtual std::string toString() const = 0;
    };

    class FresnelConductor : public Fresnel
    {
    public:
        FresnelConductor(const Spectrum& etaI,
                         const Spectrum& etaT,
                         const Spectrum& k)
            : etaI(etaI)
            , etaT(etaT)
            , k(k) {}

        Spectrum eval(Float cosThetaI) const override;

    private:
        Spectrum etaI;
        Spectrum etaT;
        Spectrum k;
    };

    class FresnelDielectric : public Fresnel
    {
    public:
        FresnelDielectric(Float etaI, Float etaT) : etaI(etaI), etaT(etaT) {}

        Spectrum eval(Float cosThetaI) const override;

    private:
        Float etaI = 0.f;
        Float etaT = 0.f;
    };

    class FresnelNoOp : public Fresnel
    {
    public:
        Spectrum eval(Float cosThetaI) const override;
    };
} // namespace idragnev::pbrt::reflection