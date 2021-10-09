#include "pbrt/samplers/HaltonSampler.hpp"
#include "pbrt/core/RNG.hpp"
#include "pbrt/core/sampling/LowDiscrepancy.hpp"
#include "pbrt/core/geometry/Bounds2.hpp"
#include "pbrt/core/math/Math.hpp"

namespace idragnev::pbrt::samplers {
    static constexpr int K_MAX_RESOLUTION = 128;

    HaltonSampler::HaltonSampler(const std::uint64_t samplesPerPixel,
                                 const Bounds2i& sampleBounds,
                                 const bool sampleAtPixelCenter)
        : GlobalSampler(samplesPerPixel)
        , sampleAtPixelCenter(sampleAtPixelCenter) {
        const Vector2i res = sampleBounds.max - sampleBounds.min;
        for (std::size_t i = 0; i < 2; ++i) {
            const int resDim = std::min(res[i], K_MAX_RESOLUTION);
            const int base = (i == 0) ? 2 : 3;

            int scale = 1;
            int exp = 0;
            while (scale < resDim) {
                scale *= base;
                ++exp;
            }

            baseScales[i] = scale;
            baseExponents[i] = exp;
        }

        sampleStride = static_cast<std::uint64_t>(baseScales[0]) *
                       static_cast<std::uint64_t>(baseScales[1]);

        multInverse[0] =
            math::multInverseCoprimes(baseScales[1], baseScales[0]);
        multInverse[1] =
            math::multInverseCoprimes(baseScales[0], baseScales[1]);
    }

    std::span<const std::uint16_t>
    HaltonSampler::permutationForDimension(const std::size_t dim) const {
        if (dim >= sampling::PRIMES_TABLE_SIZE) {
            // TODO: log
            assert(false);
            return {};
        }

        static const std::vector<std::uint16_t> radicalInversePermutations = [] {
            rng::RNG rng;
            return sampling::generateRadicalInversePermutations(rng);
        }();

        const auto permStart = radicalInversePermutations.begin() +
                               sampling::PRECEDING_PRIMES_SUMS[dim];
        const auto permSize = sampling::PRIMES[dim];

        assert((permStart + permSize - 1) < radicalInversePermutations.end());

        return std::span(permStart, permSize);
    }

    std::uint64_t
    HaltonSampler::getIndexForPixelSample(const std::uint64_t sampleNum) const {
        if (currentPixel != pixelForOffset) {
            currentPixelFirstSampleIndex = 0;

            if (sampleStride > 1) {
                Point2i pm(math::mod(currentPixel[0], K_MAX_RESOLUTION),
                           math::mod(currentPixel[1], K_MAX_RESOLUTION));

                for (int i = 0; i < 2; ++i) {
                    const std::uint64_t dimOffset =
                        (i == 0) ? sampling::inverseRadicalInverse<2>(
                                       pm[i],
                                       baseExponents[i])
                                 : sampling::inverseRadicalInverse<3>(
                                       pm[i],
                                       baseExponents[i]);

                    currentPixelFirstSampleIndex +=
                        dimOffset * (sampleStride / baseScales[i]) *
                        multInverse[i];
                }

                currentPixelFirstSampleIndex %= sampleStride;
            }

            pixelForOffset = currentPixel;
        }

        return currentPixelFirstSampleIndex + sampleNum * sampleStride;
    }

    Float HaltonSampler::sampleDimension(const std::uint64_t sampleIndex,
                                         const std::size_t dim) const {
        if (sampleAtPixelCenter && (dim <= 1)) {
            return 0.5f;
        }

        if (dim == 0) {
            return sampling::radicalInverse(dim,
                                            sampleIndex >> baseExponents[0]);
        }
        else if (dim == 1) {
            return sampling::radicalInverse(dim, sampleIndex / baseScales[1]);
        }
        else {
            return sampling::scrambledRadicalInverse(
                dim,
                sampleIndex,
                permutationForDimension(dim));
        }
    }

    std::unique_ptr<Sampler> HaltonSampler::clone(int) {
        return std::unique_ptr<Sampler>(new HaltonSampler(*this));
    }
} // namespace idragnev::pbrt::samplers