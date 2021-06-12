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

    void generateLatinHypercubeSamples(const std::span<Point2f> samples,
                                       rng::RNG& rng) {
        using rng::constants::OneMinusEpsilon;

        if (samples.empty()) {
            return;
        }

        const std::size_t samplesCount = samples.size();

        // generate samples along diagonal
        const Float invCount = 1.f / static_cast<Float>(samplesCount);
        for (std::size_t i = 0; i < samplesCount; ++i) {
            const Float x = (i + rng.uniformFloat()) * invCount;
            const Float y = (i + rng.uniformFloat()) * invCount;

            samples[i].x = std::min(x, OneMinusEpsilon);
            samples[i].y = std::min(y, OneMinusEpsilon);
        }

        // permute samples in each dimension
        for (std::size_t d = 0; d < 2; ++d) {
            for (std::size_t left = 0; left < samplesCount; ++left) {
                const auto b = static_cast<std::uint32_t>(samplesCount - left);
                const std::size_t right = left + rng.uniformUInt32(b);
                assert(right < samplesCount);

                std::swap(samples[left][d], samples[right][d]);
            }
        }
    }
} // namespace idragnev::pbrt::sampling