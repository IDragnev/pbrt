#include "pbrt/core/sampling/Sampler.hpp"
#include "pbrt/core/Camera.hpp"

#include <assert.h>

namespace idragnev::pbrt {
    CameraSample Sampler::generateCameraSample(const Point2i& rasterPoint) {
        CameraSample sample;

        // must be initialized in a specific order: (x, y) -> t -> (u, v)
        sample.pFilm = Point2f{rasterPoint} + this->generate2DSample();
        sample.time = this->generate1DSample();
        sample.pLens = this->generate2DSample();

        return sample;
    }

    void Sampler::startPixel(const Point2i& pixel) {
        currentPixel = pixel;
        currentPixelSampleIndex = 0;

        _1DSamplesArrayOffset = 0;
        _2DSamplesArrayOffset = 0;
    }

    bool Sampler::startNextSample() {
        return setSampleNumber(currentPixelSampleIndex + 1);
    }

    bool Sampler::setSampleNumber(const std::uint64_t sampleNum) {
        _1DSamplesArrayOffset = 0;
        _2DSamplesArrayOffset = 0;

        currentPixelSampleIndex = sampleNum;

        return currentPixelSampleIndex < samplesPerPixel;
    }

    void Sampler::request1DSamplesArray(const std::size_t size) {
        assert(roundSamplesCount(size) == size);

        _1DSamplesArraysSizes.push_back(size);
        _1DSamplesArray.push_back(std::vector<Float>(size * samplesPerPixel));
    }

    void Sampler::request2DSamplesArray(const std::size_t size) {
        assert(roundSamplesCount(size) == size);

        _2DSamplesArraysSizes.push_back(size);
        _2DSamplesArray.push_back(std::vector<Point2f>(size * samplesPerPixel));
    }

    Optional<std::span<const Float>>
    Sampler::getNext1DSamplesSpan(const std::size_t samplesCount) {
        if (_1DSamplesArrayOffset >= _1DSamplesArray.size()) {
            return pbrt::nullopt;
        }

        if (_1DSamplesArraysSizes[_1DSamplesArrayOffset] != samplesCount ||
            currentPixelSampleIndex >= samplesPerPixel)
        {
            assert(false);
            return pbrt::nullopt;
        }

        const auto& samples = _1DSamplesArray[_1DSamplesArrayOffset++];

        if (const auto offset = currentPixelSampleIndex * samplesCount;
            offset < samples.size())
        {
            const std::span<const Float> span(samples.begin() + offset,
                                              samplesCount);

            return pbrt::make_optional(span);
        }
        else {
            return pbrt::nullopt;
        }
    }

    Optional<std::span<const Point2f>>
    Sampler::getNext2DSamplesSpan(const std::size_t samplesCount) {
        if (_2DSamplesArrayOffset >= _2DSamplesArray.size()) {
            return pbrt::nullopt;
        }

        if (_2DSamplesArraysSizes[_2DSamplesArrayOffset] != samplesCount ||
            currentPixelSampleIndex >= samplesPerPixel)
        {
            assert(false);
            return pbrt::nullopt;
        }

        const auto& samples = _2DSamplesArray[_2DSamplesArrayOffset++];

        if (const auto offset = currentPixelSampleIndex * samplesCount;
            offset < samples.size())
        {
            const std::span<const Point2f> span(samples.begin() + offset,
                                                samplesCount);

            return pbrt::make_optional(span);
        }
        else {
            return pbrt::nullopt;
        }
    }

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