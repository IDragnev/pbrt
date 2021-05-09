#include "pbrt/core/sampling/PixelSampler.hpp"

namespace idragnev::pbrt {
    PixelSampler::PixelSampler(const std::uint64_t samplesPerPixel,
                               const std::size_t precomputedDimsCount)
        : Sampler(samplesPerPixel)
        , precomputed1DSamplesArray(precomputedDimsCount,
                                    std::vector<Float>(samplesPerPixel))
        , precomputed2DSamplesArray(precomputedDimsCount,
                                    std::vector<Point2f>(samplesPerPixel)) {}

    Float PixelSampler::generate1DSample() {
        assert(currentPixelSampleIndex < samplesPerPixel);

        if (current1DDimension < precomputed1DSamplesArray.size()) {
            return precomputed1DSamplesArray[current1DDimension++]
                                            [currentPixelSampleIndex];
        }
        else {
            return rng.uniformFloat();
        }
    }

    Point2f PixelSampler::generate2DSample() {
        assert(currentPixelSampleIndex < samplesPerPixel);

        if (current2DDimension < precomputed2DSamplesArray.size()) {
            return precomputed2DSamplesArray[current2DDimension++]
                                            [currentPixelSampleIndex];
        }
        else {
            return Point2f(rng.uniformFloat(), rng.uniformFloat());
        }
    }

    bool PixelSampler::startNextSample() {
        current1DDimension = 0;
        current2DDimension = 0;

        return Sampler::startNextSample();
    }

    bool PixelSampler::setSampleNumber(const std::uint64_t sampleNum) {
        current1DDimension = 0;
        current2DDimension = 0;

        return Sampler::setSampleNumber(sampleNum);
    }
} // namespace idragnev::pbrt