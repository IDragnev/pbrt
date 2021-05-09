#pragma once

#include "pbrt/core/sampling/Sampler.hpp"

namespace idragnev::pbrt {
    class GlobalSampler : public Sampler
    {
    public:
        GlobalSampler(const std::uint64_t samplesPerPixel)
            : Sampler(samplesPerPixel) {}

        void startPixel(const Point2i& pixel) override;

        bool startNextSample() override;
        bool setSampleNumber(const std::uint64_t sampleNum) override;

        Float generate1DSample() override;
        Point2f generate2DSample() override;

    private:
        virtual std::uint64_t
        getIndexForPixelSample(const std::uint64_t sampleNum) const = 0;

        virtual Float sampleDimension(const std::uint64_t sampleIndex,
                                      const std::size_t dim) const = 0;

        void Generate1DSamplesArray();
        void Generate2DSamplesArray();

    private:
        std::size_t sampleCurrentDim = 0;
        std::uint64_t currentPixelSampleGlobalIndex = 0;

        // specify which sample vector dimensions are generated in sample arrays
        static const std::size_t SAMPLES_ARRAY_START_DIM = 5;
        std::size_t samplesArrayEndDim = 0;
    };
} // namespace idragnev::pbrt