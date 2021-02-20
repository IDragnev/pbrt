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
        explicit SampledSpectrum(const SpectrumSamples& samples);

    private:
        static SampledSpectrum fromSamples(const SpectrumSamples& samples);
    };
}