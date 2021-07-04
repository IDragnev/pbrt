#include "doctest/doctest.h"
#include "pbrt/core/sampling/LowDiscrepancy.hpp"

namespace sampling = idragnev::pbrt::sampling;

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