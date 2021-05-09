#pragma once

#include "pbrt/core/sampling/Sampler.hpp"
#include "pbrt/core/RNG.hpp"

namespace idragnev::pbrt {

    class PixelSampler : public Sampler
    {
    public:
        PixelSampler(const std::uint64_t samplesPerPixel,
                     const std::size_t precomputedDimensionsCount);

        Float generate1DSample() override;
        Point2f generate2DSample() override;

        bool startNextSample() override;
        bool setSampleNumber(const std::uint64_t sampleNum) override;

    protected:
        std::vector<std::vector<Float>> precomputed1DSamplesArray;
        std::vector<std::vector<Point2f>> precomputed2DSamplesArray;
        std::size_t current1DDimension = 0;
        std::size_t current2DDimension = 0;

        rng::RNG rng;
    };
} // namespace idragnev::pbrt