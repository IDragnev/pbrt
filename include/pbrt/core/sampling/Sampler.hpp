#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/Optional.hpp"
#include "pbrt/core/math/Point2.hpp"

#include <vector>
#include <memory>
#include <span>

namespace idragnev::pbrt {
    struct CameraSample;

    class Sampler
    {
    public:
        Sampler(const std::uint64_t samplesPerPixel)
            : samplesPerPixel(samplesPerPixel) {}
        virtual ~Sampler() = default;

        virtual std::unique_ptr<Sampler> clone(int seed) = 0;

        virtual void startPixel(const Point2i& p);

        virtual Float generate1DSample() = 0;
        virtual Point2f generate2DSample() = 0;
        CameraSample generateCameraSample(const Point2i& rasterPoint);

        void request1DSamplesArray(const std::size_t size);
        void request2DSamplesArray(const std::size_t size);

        Optional<std::span<const Float>>
        getNext1DSamplesSpan(const std::size_t samplesCount);
        Optional<std::span<const Point2f>>
        getNext2DSamplesSpan(const std::size_t samplesCount);

        virtual std::size_t roundSamplesCount(const std::size_t n) const {
            return n;
        }

        virtual bool startNextSample();
        virtual bool setSampleNumber(const std::uint64_t sampleNum);

        std::uint64_t currentSampleNumber() const {
            return currentPixelSampleIndex;
        }

        std::uint64_t numberOfSamplesPerPixel() const {
            return samplesPerPixel;
        }

    protected:
        Point2i currentPixel;
        std::uint64_t currentPixelSampleIndex = 0;
        std::uint64_t samplesPerPixel = 1;

        std::vector<std::size_t> _1DSamplesArraysSizes;
        std::vector<std::size_t> _2DSamplesArraysSizes;
        std::vector<std::vector<Float>> _1DSamplesArray;
        std::vector<std::vector<Point2f>> _2DSamplesArray;

    private:
        std::size_t _1DSamplesArrayOffset = 0;
        std::size_t _2DSamplesArrayOffset = 0;
    };
} // namespace idragnev::pbrt