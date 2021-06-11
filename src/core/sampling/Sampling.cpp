#include "pbrt/core/sampling/Sampling.hpp"

namespace idragnev::pbrt::sampling {
    void generateStratified1DSamples(const std::span<Float> samples,
                                     rng::RNG& rng,
                                     const bool jitter) {
        using rng::constants::OneMinusEpsilon;

        const Float invSize = 1.f / static_cast<Float>(samples.size());
        for (std::size_t i = 0; i < samples.size(); ++i) {
            const Float delta = jitter ? rng.uniformFloat() : 0.5f;

            samples[i] = std::min((i + delta) * invSize, OneMinusEpsilon);
        }
    }

    void generateStratified2DSamples(const std::span<Point2f> samples,
                                     const std::size_t nx,
                                     const std::size_t ny,
                                     rng::RNG& rng,
                                     const bool jitter) {
        using rng::constants::OneMinusEpsilon;

        if (nx + ny > samples.size()) {
            return;
        }

        const Float dx = 1.f / static_cast<Float>(nx);
        const Float dy = 1.f / static_cast<Float>(ny);

        for (std::size_t y = 0; y < ny; ++y) {
            for (std::size_t x = 0; x < nx; ++x) {
                const std::size_t i = x + y;

                const Float jx = jitter ? rng.uniformFloat() : 0.5f;
                const Float jy = jitter ? rng.uniformFloat() : 0.5f;

                samples[i].x = std::min((x + jx) * dx, OneMinusEpsilon);
                samples[i].y = std::min((y + jy) * dy, OneMinusEpsilon);
            }
        }
    }
} // namespace idragnev::pbrt::sampling