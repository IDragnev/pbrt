#include "pbrt/samplers/StratifiedSampler.hpp"
#include "pbrt/core/sampling/Sampling.hpp"

namespace idragnev::pbrt::samplers {
    StratifiedSampler::StratifiedSampler(
        const std::size_t xPixelSamplesCount,
        const std::size_t yPixelSamplesCount,
        const bool jitterSamples,
        const std::size_t precomputedDimensionsCount)
        : PixelSampler(xPixelSamplesCount * yPixelSamplesCount,
                       precomputedDimensionsCount)
        , xPixelSamplesCount(xPixelSamplesCount)
        , yPixelSamplesCount(yPixelSamplesCount)
        , jitterSamples(jitterSamples) {}

    void StratifiedSampler::startPixel(const Point2i& p) {
        generatePrecomputedSamplesArrays();
        generateArrayedSamplesArrays();

        PixelSampler::startPixel(p);
    }

    void StratifiedSampler::generatePrecomputedSamplesArrays()
    {
        for (std::vector<Float>& samples : precomputed1DSamplesArray) {
            const auto span = std::span<Float>(samples);

            sampling::generateStratified1DSamples(
                span,
                jitterSamples ? pbrt::make_optional<rng::RNG&>(rng)
                              : pbrt::nullopt);

            sampling::shuffleSamples(span, 1, rng);
        }

        for (std::vector<Point2f>& samples : precomputed2DSamplesArray) {
            const auto span = std::span<Point2f>(samples);

            sampling::generateStratified2DSamples(
                span,
                xPixelSamplesCount,
                yPixelSamplesCount,
                jitterSamples ? pbrt::make_optional<rng::RNG&>(rng)
                              : pbrt::nullopt);

            sampling::shuffleSamples(span, 1, rng);
        }
    }

    void StratifiedSampler::generateArrayedSamplesArrays()
    {
        for (std::size_t i = 0; i < _1DSamplesArraysSizes.size(); ++i) {
            const std::size_t chunkSize = _1DSamplesArraysSizes[i];
            std::vector<Float>& arrayedSamples = _1DSamplesArray[i];

            for (std::uint64_t sampleNum = 0; sampleNum < samplesPerPixel;
                 ++sampleNum) {
                const auto chunkStart =
                    arrayedSamples.begin() + sampleNum * chunkSize;
                const std::span<Float> chunk = std::span(chunkStart, chunkSize);

                sampling::generateStratified1DSamples(
                    chunk,
                    jitterSamples ? pbrt::make_optional<rng::RNG&>(rng)
                                  : pbrt::nullopt);

                sampling::shuffleSamples(chunk, 1, rng);
            }
        }

        for (std::size_t i = 0; i < _2DSamplesArraysSizes.size(); ++i) {
            const std::size_t chunkSize = _2DSamplesArraysSizes[i];
            std::vector<Point2f>& arrayedSamples = _2DSamplesArray[i];

            for (std::uint64_t sampleNum = 0; sampleNum < samplesPerPixel;
                 ++sampleNum) {
                const auto chunkStart =
                    arrayedSamples.begin() + sampleNum * chunkSize;
                const auto chunk = std::span<Point2f>(chunkStart, chunkSize);

                sampling::generateLatinHypercubeSamples(chunk, rng);
            }
        }
    }

    std::unique_ptr<Sampler> StratifiedSampler::clone(int seed) {
        auto sampler = std::make_unique<StratifiedSampler>(*this);
        sampler->rng.setSequence(seed);

        return sampler;
    }
} // namespace idragnev::pbrt::samplers