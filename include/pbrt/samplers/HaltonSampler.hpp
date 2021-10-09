#pragma once

#include "pbrt/core/sampling/GlobalSampler.hpp"

#include <span>

namespace idragnev::pbrt::samplers {
    class HaltonSampler : public GlobalSampler
    {
    public:
        HaltonSampler(const std::uint64_t samplesPerPixel,
                      const Bounds2i& sampleBounds,
                      const bool sampleAtCenter = false);

        std::unique_ptr<Sampler> clone(int seed) override;

    private:
        std::uint64_t
        getIndexForPixelSample(const std::uint64_t sampleNum) const override;
        Float sampleDimension(const std::uint64_t sampleIndex,
                              const std::size_t dim) const override;

        std::span<const std::uint16_t>
        permutationForDimension(const std::size_t dim) const;

    private:
        Point2i baseScales;
        Point2i baseExponents;

        std::uint64_t sampleStride = 0;
        std::uint64_t multInverse[2] = {0, 0};

        bool sampleAtPixelCenter = false;
        mutable Point2i pixelForOffset =
            Point2i(std::numeric_limits<Point2i::UnderlyingType>::max(),
                    std::numeric_limits<Point2i::UnderlyingType>::max());
        mutable std::uint64_t currentPixelFirstSampleIndex = 0;
    };
} // namespace idragnev::pbrt::samplers