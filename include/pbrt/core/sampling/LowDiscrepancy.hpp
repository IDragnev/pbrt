#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/RNG.hpp"

#include <span>
#include <vector>

namespace idragnev::pbrt::sampling {
    static inline constexpr std::size_t PRIMES_TABLE_SIZE = 1000u;
    extern const std::uint32_t PRIMES[PRIMES_TABLE_SIZE];
    extern const std::uint32_t PRECEDING_PRIMES_SUMS[PRIMES_TABLE_SIZE];

    inline constexpr std::uint32_t reverseBits32(std::uint32_t n) noexcept {
        n = (n << 16) | (n >> 16);
        n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
        n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
        n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
        n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);

        return n;
    }

    inline constexpr std::uint64_t reverseBits64(std::uint64_t n) noexcept {
        const std::uint64_t rightRev =
            reverseBits32(static_cast<std::uint32_t>(n));
        const std::uint64_t leftRev =
            reverseBits32(static_cast<std::uint32_t>(n >> 32));

        return (rightRev << 32) | leftRev;
    }

    // Computes the radical inverse of `a` to the base Primes[baseIndex]
    // where Primes = [2, 3, 5, 7 ..]
    Float radicalInverse(const std::size_t baseIndex, const std::uint64_t a);

    // The inverse of `radicalInverse` but taking the inverse digits
    // representation of the corresponding number `a` (before being multiplied
    // by 1/(b^n)). `numberOfDigits` is used so trailing zeroes are not lost
    // with the inverse digits representation.
    template <std::size_t Base>
    inline std::size_t
    inverseRadicalInverse(std::uint64_t inverse,
                          const std::size_t numberOfDigits) {
        std::size_t index = 0;

        for (std::size_t i = 0; i < numberOfDigits; ++i) {
            const std::uint64_t digit = inverse % Base;

            inverse /= Base;
            index = index * Base + digit;
        }

        return index;
    }

    std::vector<std::uint16_t> generateRadicalInversePermutations(rng::RNG& rng);

    Float scrambledRadicalInverse(const std::size_t baseIndex,
                                  const std::uint64_t a,
                                  const std::span<const uint16_t> perm);
} // namespace idragnev::pbrt::sampling