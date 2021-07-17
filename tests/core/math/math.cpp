#include "doctest/doctest.h"

#include "pbrt/core/math/Matrix4x4.hpp"
#include "pbrt/core/math/Vector3.hpp"

namespace pbrt = idragnev::pbrt;

TEST_CASE("mod with integers") {
    CHECK(pbrt::math::mod(26, 26) == 0);

    for (int i = 0; i < 26; ++i) {
        CHECK(pbrt::math::mod(i, 26) == i);
    }

    CHECK(pbrt::math::mod(-13, 26) == 13);
}

TEST_CASE("extended gcd") {
    std::int64_t x = 0;
    std::int64_t y = 0;
    pbrt::math::extendedGCD(240, 46, x, y);

    CHECK(x == -9);
    CHECK(y == 47);
}

TEST_CASE("multiplicative inverse with coprimes") {
    CHECK(pbrt::math::multInverseCoprimes(7, 26) == 15);
    CHECK(pbrt::math::multInverseCoprimes(11, 26) == 19);
    CHECK(pbrt::math::multInverseCoprimes(17, 26) == 23);
}