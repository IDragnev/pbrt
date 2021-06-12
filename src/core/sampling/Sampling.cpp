#include "pbrt/core/sampling/Sampling.hpp"

namespace idragnev::pbrt::sampling {
    void generateStratified1DSamples(const std::span<Float> samples,
                                     Optional<rng::RNG&> rng) {
        using rng::constants::OneMinusEpsilon;

        const bool jitter = rng.has_value();
        const Float invSize = 1.f / static_cast<Float>(samples.size());

        for (std::size_t i = 0; i < samples.size(); ++i) {
            const Float delta = jitter ? rng->uniformFloat() : 0.5f;

            samples[i] = std::min((i + delta) * invSize, OneMinusEpsilon);
        }
    }

    void generateStratified2DSamples(const std::span<Point2f> samples,
                                     const std::size_t nx,
                                     const std::size_t ny,
                                     Optional<rng::RNG&> rng) {
        using rng::constants::OneMinusEpsilon;

        if (nx + ny > samples.size()) {
            return;
        }

        const bool jitter = rng.has_value();
        const Float dx = 1.f / static_cast<Float>(nx);
        const Float dy = 1.f / static_cast<Float>(ny);

        for (std::size_t y = 0; y < ny; ++y) {
            for (std::size_t x = 0; x < nx; ++x) {
                const std::size_t i = x + y;

                const Float jx = jitter ? rng->uniformFloat() : 0.5f;
                const Float jy = jitter ? rng->uniformFloat() : 0.5f;

                samples[i].x = std::min((x + jx) * dx, OneMinusEpsilon);
                samples[i].y = std::min((y + jy) * dy, OneMinusEpsilon);
            }
        }
    }

    void generateLatinHypercubeSamples(const std::span<Float> samples,
                                       const std::size_t dims,
                                       rng::RNG& rng) {
        using rng::constants::OneMinusEpsilon;

        const std::size_t blocksCount = samples.size() / dims;

        // generate samples along diagonal
        const Float invBlocksCount = 1.f / blocksCount;
        for (std::size_t block = 0; block < blocksCount; ++block) {
            for (std::size_t d = 0; d < dims; ++d) {
                const Float value = (block + rng.uniformFloat()) * invBlocksCount;

                samples[block * dims + d] = std::min(value, OneMinusEpsilon);
            }
        }

        // permute samples in each dimension
        for (std::size_t d = 0; d < dims; ++d) {
            for (std::size_t left = 0; left < blocksCount; ++left) {
                const std::size_t right =
                    left + rng.uniformUInt32(
                               static_cast<std::uint32_t>(blocksCount - left));
                assert(right < blocksCount);

                std::swap(samples[left * dims + d], samples[right * dims + d]);
            }
        }
    }
} // namespace idragnev::pbrt::sampling