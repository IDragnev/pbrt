#include "pbrt/core/SampledSpectrum.hpp"

#include <vector>
#include <algorithm>

namespace idragnev::pbrt {
    // Inclusive wavelength region [start, end]
    struct WavelengthRegion
    {
        Float lambdaStart = 0.f;
        Float lambdaEnd = 0.f;
    };

    static constexpr WavelengthRegion SAMPLED_WAVELENGTH_REGION =
        WavelengthRegion{
            .lambdaStart = 400.f,
            .lambdaEnd = 700.f,
        };

    void sortSpectrumSamples(const std::span<Float> lambdas,
                             const std::span<Float> values);
    Float averageSpectrumSamples(const std::span<const Float> lambdas,
                                 const std::span<const Float> values,
                                 const WavelengthRegion& region);

    SampledSpectrum::SampledSpectrum(const SpectrumSamples& samples)
        : SampledSpectrum(fromSamples(samples)) {}

    SampledSpectrum
    SampledSpectrum::fromSamples(const SpectrumSamples& spectrumSamples) {
        const std::span<const Float> lambdas = spectrumSamples.lambdas;
        const std::span<const Float> values = spectrumSamples.values;
        assert(lambdas.size() == values.size());

        if (!std::is_sorted(lambdas.begin(), lambdas.end())) {
            std::vector<Float> sortedLambdas(lambdas.begin(), lambdas.end());
            std::vector<Float> sortedVals(values.begin(), values.end());
            sortSpectrumSamples(sortedLambdas, sortedVals);

            return fromSamples(SpectrumSamples{
                .lambdas = sortedLambdas,
                .values = sortedVals,
            });
        }

        std::array<Float, SampledSpectrum::NumberOfSamples> samples;
        const Float samplesCount = static_cast<Float>(samples.size());
        for (std::size_t i = 0; i < samples.size(); ++i) {
            const Float lambda0 = lerp(static_cast<Float>(i) / samplesCount,
                                       SAMPLED_WAVELENGTH_REGION.lambdaStart,
                                       SAMPLED_WAVELENGTH_REGION.lambdaEnd);
            const Float lambda1 = lerp(static_cast<Float>(i + 1) / samplesCount,
                                       SAMPLED_WAVELENGTH_REGION.lambdaStart,
                                       SAMPLED_WAVELENGTH_REGION.lambdaEnd);

            samples[i] = averageSpectrumSamples(lambdas,
                                                values,
                                                WavelengthRegion{
                                                    .lambdaStart = lambda0,
                                                    .lambdaEnd = lambda1,
                                                });
        }

        return SampledSpectrum{samples};
    }

    Float averageSpectrumSamples(const std::span<const Float> lambdas,
                                 const std::span<const Float> values,
                                 const WavelengthRegion& region) {
        assert(lambdas.size() == values.size());
        assert(std::is_sorted(lambdas.begin(), lambdas.end()));
        assert(region.lambdaStart < region.lambdaEnd);

        if (lambdas.size() == 1) {
            return values.front();
        }
        if (region.lambdaEnd <= lambdas.front()) {
            return values.front();
        }
        if (region.lambdaStart >= lambdas.back()) {
            return values.back();
        }

        Float sum = 0;
        if (region.lambdaStart < lambdas.front()) {
            sum += values.front() * (lambdas.front() - region.lambdaStart);
        }
        if (region.lambdaEnd > lambdas.back()) {
            sum += values.back() * (region.lambdaEnd - lambdas.back());
        }

        const auto lerpSamplesSegment = [lambdas, values](const Float w,
                                                          const std::size_t i) {
            return lerp((w - lambdas[i]) / (lambdas[i + 1] - lambdas[i]),
                        values[i],
                        values[i + 1]);
        };

        // find the first lambdas segment {lambda i, lambda i + 1}
        // that overlaps `region` (lambdas[i] <= region.lambdaStart <= lambdas[i + 1])
        const auto lambdaIplus1Pos =
            std::lower_bound(lambdas.begin(), lambdas.end(), region.lambdaStart);
        assert(lambdaIplus1Pos != lambdas.begin());
        assert(lambdaIplus1Pos != lambdas.end());

        const auto lambdaIplus1Index = lambdaIplus1Pos - lambdas.begin();
        for (std::size_t i = static_cast<std::size_t>(lambdaIplus1Index) - 1;
             i + 1 < lambdas.size() && lambdas[i] <= region.lambdaEnd;
             ++i)
        {
            const Float segmentLambdaStart = std::max(region.lambdaStart, lambdas[i]);
            const Float segmentLambdaEnd = std::min(region.lambdaEnd, lambdas[i + 1]);
            const Float segmentAverageValue =
                0.5f * (lerpSamplesSegment(segmentLambdaStart, i) +
                        lerpSamplesSegment(segmentLambdaEnd, i));
            sum += segmentAverageValue * (segmentLambdaEnd - segmentLambdaStart);
        }

        return sum / (region.lambdaEnd - region.lambdaStart);
    }

    void sortSpectrumSamples(const std::span<Float> lambdas,
                             const std::span<Float> values) {
        assert(lambdas.size() == values.size());

        std::vector<std::pair<Float, Float>> pairs(lambdas.size());
        for (std::size_t i = 0; i < pairs.size(); ++i) {
            pairs[i] = std::make_pair(lambdas[i], values[i]);
        }
        std::sort(pairs.begin(), pairs.end());

        for (std::size_t i = 0; i < pairs.size(); ++i) {
            lambdas[i] = pairs[i].first;
            values[i] = pairs[i].second;
        }
    }
} // namespace idragnev::pbrt