#pragma once

#include "pbrt/core/math/Fwd.hpp"
#include "pbrt/core/math/Point2.hpp"
#include "pbrt/core/RNG.hpp"
#include "pbrt/core/Optional.hpp"

#include <span>

namespace idragnev::pbrt::sampling {
    inline Point2f concentricSampleDisk(const Point2f&) {
        // TODO
        return Point2f{};
    }

    // Generates stratified [0, 1) sample values in `samples`,
    // optionally jittering the sample values if `rng` is provided.
    void generateStratified1DSamples(const std::span<Float> samples,
                                     Optional<rng::RNG&> rng);
    // Generates stratified [0, 1)x[0, 1) sample values in `samples`,
    // optionally jittering the sample values if `rng` is provided.
    // `nx` and `ny` specify the rows and columns of the 2D sample grid.
    // Nothing is done if nx + ny > samples.size.
    void generateStratified2DSamples(const std::span<Point2f> samples,
                                     const std::size_t nx,
                                     const std::size_t ny,
                                     Optional<rng::RNG&> rng);

    // Generates [0, 1)x[0, 1) sample values in `samples` using
    // the Latin Hypercube approach. Jitters each sample value using `rng`.
    void generateLatinHypercubeSamples(const std::span<Point2f> samples,
                                       rng::RNG& rng);

    template <typename T>
    void shuffleSamples(const std::span<T> samples,
                        const std::size_t blockSize,
                        rng::RNG& rng) {
        const std::size_t blocksCount = samples.size() / blockSize;

        for (std::size_t left = 0; left < blocksCount; ++left) {
            const auto b = static_cast<std::uint32_t>(blocksCount - left);
            const std::size_t right = left + rng.uniformUInt32(b);
            assert(right < blocksCount);

            for (std::size_t i = 0; i < blockSize; ++i) {
                std::swap(samples[blockSize * left + i],
                          samples[blockSize * right + i]);
            }
        }
    }
} // namespace idragnev::pbrt::sampling