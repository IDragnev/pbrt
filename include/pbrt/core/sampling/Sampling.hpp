#pragma once

#include "pbrt/core/math/Fwd.hpp"
#include "pbrt/core/math/Point2.hpp"
#include "pbrt/core/RNG.hpp"

#include <span>

namespace idragnev::pbrt::sampling {
    inline Point2f concentricSampleDisk(const Point2f&) {
        // TODO
        return Point2f{};
    }

    void generateStratified1DSamples(const std::span<Float> samples,
                                     rng::RNG& rng,
                                     const bool jitter = true);
    void generateStratified2DSamples(const std::span<Point2f> samples,
                                     const std::size_t nx,
                                     const std::size_t ny,
                                     rng::RNG& rng,
                                     const bool jitter = true);

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