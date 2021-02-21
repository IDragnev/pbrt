#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/CoefficientSpectrum.hpp"

#include <span>

namespace idragnev::pbrt {
    struct SpectrumSamples
    {
        std::span<const Float> lambdas;
        std::span<const Float> values;
    };

    class SampledSpectrum : public CoefficientSpectrum<60>
    {
    public:
        using CoefficientSpectrum::CoefficientSpectrum;

        static SampledSpectrum fromSamples(const SpectrumSamples& samples);
        static constexpr SampledSpectrum fromSortedSamples(const SpectrumSamples& samples);

        std::array<Float, 3> toXYZ() const;
        Float y() const;
    };
}