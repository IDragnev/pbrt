#pragma once

#include "pbrt/core/sampling/PixelSampler.hpp"

namespace idragnev::pbrt::samplers {
    class StratifiedSampler : public PixelSampler
    {
    public:
        StratifiedSampler(const std::size_t xPixelSamplesCount,
                          const std::size_t yPixelSamplesCount,
                          const bool jitterSamples,
                          const std::size_t precomputedDimensionsCount);

        std::unique_ptr<Sampler> clone(int seed) override;

        void startPixel(const Point2i& p) override;

    private:
        void generatePrecomputedSamplesArrays();
        void generateArrayedSamplesArrays();

    private:
        std::size_t xPixelSamplesCount = 0;
        std::size_t yPixelSamplesCount = 0;
        bool jitterSamples = false;
    };
} // namespace idragnev::pbrt::samplers