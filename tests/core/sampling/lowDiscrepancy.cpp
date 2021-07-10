#include "doctest/doctest.h"
#include "pbrt/core/sampling/LowDiscrepancy.hpp"
#include "pbrt/core/sampling/Sampling.hpp"

#include <algorithm>
#include <unordered_set>

namespace pbrt = idragnev::pbrt;
namespace sampling = idragnev::pbrt::sampling;
namespace rng = idragnev::pbrt::rng;

TEST_CASE("reverseBits32") {
    CHECK(sampling::reverseBits32(0) == 0);
    CHECK(sampling::reverseBits32(0x0000000f) == 0xf0000000);
    CHECK(sampling::reverseBits32(0x00f000f0) == 0x0f000f00);
    CHECK(sampling::reverseBits32(0x00fff000) == 0x000fff00);
}

TEST_CASE("reverseBits64") {
    CHECK(sampling::reverseBits64(0) == 0);
    CHECK(sampling::reverseBits64(0x00f000000000000f) == 0xf000000000000f00);
    CHECK(sampling::reverseBits64(0x00f000f0000000f0) == 0x0f0000000f000f00);
}

// the base = 2 case can be implemented more efficiently than the general case
// so it should be tested explicitly
TEST_CASE("radicalInverse with base 2") {
    const std::size_t baseIndex = 0;

    CHECK(sampling::radicalInverse(baseIndex, 0) == 0);
    CHECK(sampling::radicalInverse(baseIndex, 1) == doctest::Approx(0.5f));
    CHECK(sampling::radicalInverse(baseIndex, 2) == doctest::Approx(0.25f));
    CHECK(sampling::radicalInverse(baseIndex, 3) == doctest::Approx(0.75f));
    CHECK(sampling::radicalInverse(baseIndex, 4) == doctest::Approx(0.125f));
}

TEST_CASE("radicalInverse with base 3") {
    const std::size_t baseIndex = 1;

    // clang-format off
    CHECK(sampling::radicalInverse(baseIndex, 0) == 0.f);
    CHECK(sampling::radicalInverse(baseIndex, 1)  == doctest::Approx(0.3333333f));
    CHECK(sampling::radicalInverse(baseIndex, 2)  == doctest::Approx(0.6666666f));
    CHECK(sampling::radicalInverse(baseIndex, 3)  == doctest::Approx(0.1111111f));
    CHECK(sampling::radicalInverse(baseIndex, 10) == doctest::Approx(0.3703703f));
    // clang-format on
}

TEST_CASE("radical inverse permuatations generation") {
    rng::RNG rng;
    const auto permutations = sampling::generateRadicalInversePermutations(rng);

    for (auto permStart = permutations.begin();
         const auto prime : sampling::PRIMES) {
        const auto permElements =
            std::unordered_set<std::uint16_t>{permStart, permStart + prime};

        REQUIRE(permElements.size() == prime);
        CHECK(std::all_of(permElements.begin(),
                          permElements.end(),
                          [prime](auto pi) { return pi < prime; }));

        permStart += prime;
    }
}

TEST_CASE("scrambledRadicalInverse") {
    for (std::size_t dim = 0; dim < 128; ++dim) {
        const auto base = sampling::PRIMES[dim];
        const auto perm = [base, dim] {
            std::vector<std::uint16_t> perm(base, 0u);
            for (std::uint32_t i = 0; i < base; ++i) {
                perm[i] = static_cast<std::uint16_t>(base - 1 - i);
            }

            rng::RNG rng(dim);
            sampling::shuffle(std::span(perm), 1, rng);

            return perm;
        }();

        const auto indices = {0, 1, 2, 1151, 32351, 4363211, 681122};

        SUBCASE("compared to the pbrt-v2 implementation") {
            for (const std::uint32_t index : indices) {
                pbrt::Float val = 0.f;
                pbrt::Float invBase = 1.f / base;
                pbrt::Float invBi = invBase;
                std::uint32_t n = index;

                while (n > 0) {
                    std::uint32_t d_i = perm[n % base];
                    val += d_i * invBi;
                    n = static_cast<std::uint32_t>(n * invBase);
                    invBi *= invBase;
                }

                // For the case where the permutation table permutes the digit 0
                // to another digit, account for the infinite sequence of that
                // digit trailing at the end of the radical inverse value.
                val += perm[0] * base / (base - 1.0f) * invBi;

                const pbrt::Float result =
                    sampling::scrambledRadicalInverse(dim,
                                                      index,
                                                      std::span(perm));

                WARN_MESSAGE(result == doctest::Approx(val).epsilon(1e-5),
                             "index = ",
                             index);
            }
        }

        SUBCASE("compared to a naive \"loop over all the bits in the index\" "
                "approach") {
            for (const std::uint32_t index : indices) {
                pbrt::Float val = 0.f;
                pbrt::Float invBase = 1.f / base;
                pbrt::Float invBi = invBase;

                std::uint32_t a = index;
                for (int i = 0; i < 32; ++i) {
                    std::uint32_t d_i = perm[a % base];
                    a /= base;
                    val += d_i * invBi;
                    invBi *= invBase;
                }

                const pbrt::Float result =
                    sampling::scrambledRadicalInverse(dim,
                                                      index,
                                                      std::span(perm));

                WARN_MESSAGE(doctest::Approx(val).epsilon(1e-5) == result,
                             " index = ",
                             index);
            }
        }
    }
}