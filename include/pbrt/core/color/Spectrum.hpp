#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/color/CoefficientSpectrum.hpp"

#include <span>

namespace idragnev::pbrt {
    class RGBSpectrum;
    class SampledSpectrum;

#ifdef PBRT_SAMPLED_SPECTRUM
    using Spectrum = SampledSpectrum;
#else
    using Spectrum = RGBSpectrum;
#endif

    constexpr std::array<Float, 3>
    XYZToRGB(const std::array<Float, 3>& xyz) noexcept;
    constexpr std::array<Float, 3>
    RGBToXYZ(const std::array<Float, 3>& rgb) noexcept;

    enum class SpectrumType
    {
        Reflectance,
        Illuminant,
    };

    struct SpectrumSamples
    {
        std::span<const Float> lambdas;
        std::span<const Float> values;
    };

    class RGBSpectrum : public CoefficientSpectrum<3>
    {
    public:
        using CoefficientSpectrum::CoefficientSpectrum;
        RGBSpectrum(const CoefficientSpectrum<RGBSpectrum::NumberOfSamples>& cs)
            : CoefficientSpectrum(cs) {}

        static RGBSpectrum fromSamples(SpectrumSamples samples);
        static RGBSpectrum fromSortedSamples(SpectrumSamples samples);
        static RGBSpectrum fromRGB(const std::array<Float, 3>& rgb) {
            return RGBSpectrum(rgb);
        }
        static RGBSpectrum fromXYZ(const std::array<Float, 3>& xyz) {
            return fromRGB(XYZToRGB(xyz));
        }

        Float y() const {
            return 0.212671f * (*this)[0] +
                   0.715160f * (*this)[1] +
                   0.072169f * (*this)[2];
        }
        [[nodiscard]] std::array<Float, 3> toXYZ() const {
            return RGBToXYZ(this->toRGB());
        }
        [[nodiscard]] std::array<Float, 3> toRGB() const {
            return {(*this)[0], (*this)[1], (*this)[2]};
        }

        [[nodiscard]] RGBSpectrum toRGBSpectrum() const { return *this; }
    };

    class SampledSpectrum : public CoefficientSpectrum<60>
    {
    public:
        using CoefficientSpectrum::CoefficientSpectrum;
        SampledSpectrum(
            const CoefficientSpectrum<SampledSpectrum::NumberOfSamples>& cs)
            : CoefficientSpectrum(cs) {}

        static SampledSpectrum fromSamples(SpectrumSamples samples);
        static constexpr SampledSpectrum
        fromSortedSamples(SpectrumSamples samples);

        static SampledSpectrum
        fromXYZ(const std::array<Float, 3>& xyz,
                const SpectrumType type = SpectrumType::Reflectance) {
            return fromRGB(XYZToRGB(xyz), type);
        }
        static SampledSpectrum fromRGB(const std::array<Float, 3>& rgb,
                                       const SpectrumType type) {
            return type == SpectrumType::Reflectance ? fromRGBReflectance(rgb)
                                                     : fromRGBIlluminant(rgb);
        }
        static SampledSpectrum
        fromRGBReflectance(const std::array<Float, 3>& rgb);
        static SampledSpectrum
        fromRGBIlluminant(const std::array<Float, 3>& rgb);

        Float y() const;
        [[nodiscard]] std::array<Float, 3> toXYZ() const;
        [[nodiscard]] std::array<Float, 3> toRGB() const {
            return XYZToRGB(this->toXYZ());
        }

        [[nodiscard]] RGBSpectrum toRGBSpectrum() const {
            return RGBSpectrum::fromRGB(this->toRGB());
        }
    };

    inline constexpr std::array<Float, 3>
    XYZToRGB(const std::array<Float, 3>& xyz) noexcept {
        std::array<Float, 3> rgb;
        // clang-fomat off
        rgb[0] =  3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
        rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
        rgb[2] =  0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
        // clang-format on

        return rgb;
    }

    inline constexpr std::array<Float, 3>
    RGBToXYZ(const std::array<Float, 3>& rgb) noexcept {
        std::array<Float, 3> xyz;
        xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
        xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
        xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];

        return xyz;
    }
}