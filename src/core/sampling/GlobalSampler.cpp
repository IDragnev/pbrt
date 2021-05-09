#include "pbrt/core/sampling/GlobalSampler.hpp"

namespace idragnev::pbrt {
    void GlobalSampler::startPixel(const Point2i& pixel) {
        Sampler::startPixel(pixel);

        sampleCurrentDim = 0;
        currentPixelSampleGlobalIndex = getIndexForPixelSample(0);

        samplesArrayEndDim = SAMPLES_ARRAY_START_DIM + _1DSamplesArray.size() +
                             2 * _2DSamplesArray.size();

        Generate1DSamplesArray();
        Generate2DSamplesArray();
    }

    void GlobalSampler::Generate1DSamplesArray() {
        for (std::size_t i = 0; i < _1DSamplesArray.size(); ++i) {
            std::vector<Float>& samples = _1DSamplesArray[i];
            const auto samplesCount = samples.size();

            for (std::size_t j = 0; j < samplesCount; ++j) {
                const std::uint64_t index = getIndexForPixelSample(j);
                const std::size_t d = SAMPLES_ARRAY_START_DIM + i;

                samples[j] = sampleDimension(index, d);
            }
        }
    }

    void GlobalSampler::Generate2DSamplesArray() {
        std::size_t dim = SAMPLES_ARRAY_START_DIM + _1DSamplesArraysSizes.size();

        for (std::size_t i = 0; i < _2DSamplesArray.size(); ++i) {
            std::vector<Point2f>& samples = _2DSamplesArray[i];
            const auto samplesCount = samples.size();

            for (std::size_t j = 0; j < samplesCount; ++j) {
                const std::uint64_t index = getIndexForPixelSample(j);

                samples[j].x = sampleDimension(index, dim);
                samples[j].y = sampleDimension(index, dim + 1);
            }

            dim += 2;
        }

        assert(samplesArrayEndDim == dim);
    }

    bool GlobalSampler::startNextSample() {
        sampleCurrentDim = 0;
        currentPixelSampleGlobalIndex =
            getIndexForPixelSample(currentPixelSampleIndex + 1);

        return Sampler::startNextSample();
    }

    bool GlobalSampler::setSampleNumber(const std::uint64_t sampleNum) {
        sampleCurrentDim = 0;
        currentPixelSampleGlobalIndex = getIndexForPixelSample(sampleNum);

        return Sampler::setSampleNumber(sampleNum);
    }

    Float GlobalSampler::generate1DSample() {
        if (sampleCurrentDim >= SAMPLES_ARRAY_START_DIM &&
            sampleCurrentDim < samplesArrayEndDim)
        {
            sampleCurrentDim = samplesArrayEndDim;
        }

        return sampleDimension(currentPixelSampleGlobalIndex,
                               sampleCurrentDim++);
    }

    Point2f GlobalSampler::generate2DSample() {
        if (sampleCurrentDim >= SAMPLES_ARRAY_START_DIM &&
            sampleCurrentDim < samplesArrayEndDim)
        {
            sampleCurrentDim = samplesArrayEndDim;
        }

        const auto p = Point2f(
            sampleDimension(currentPixelSampleGlobalIndex, sampleCurrentDim),
            sampleDimension(currentPixelSampleGlobalIndex,
                            sampleCurrentDim + 1));

        sampleCurrentDim += 2;

        return p;
    }
} // namespace idragnev::pbrt