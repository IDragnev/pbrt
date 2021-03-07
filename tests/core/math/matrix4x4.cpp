#include "doctest/doctest.h"

#include "pbrt/core/math/Matrix4x4.hpp"
#include "pbrt/core/math/Vector3.hpp"

namespace pbrt = idragnev::pbrt;

TEST_CASE("inverse") {
    SUBCASE("of identity is identity") {
        const auto id = pbrt::math::Matrix4x4::identity();

        REQUIRE(inverse(id) == id);
    }

    SUBCASE("of a nontrivial matrix") {
        const auto v = pbrt::Vector3f{2.f, 3.f, 4.f};
        // clang-format off
        const auto m = pbrt::math::Matrix4x4{ 
            1.f, 0.f, 0.f, v.x,
            0.f, 1.f, 0.f, v.y,
            0.f, 0.f, 1.f, v.z,
            0.f, 0.f, 0.f, 1.f
        };
        const auto expected = pbrt::math::Matrix4x4{
            1.f, 0.f, 0.f, -v.x,
            0.f, 1.f, 0.f, -v.y,
            0.f, 0.f, 1.f, -v.z,
            0.f, 0.f, 0.f, 1.f
        };
        // clang-format on

        CHECK(inverse(m) == expected);
    }
}

TEST_CASE("transpose") {
    // clang-format off
    const auto m = pbrt::math::Matrix4x4{
        1.f,  2.f,  3.f,  4.f,
        5.f,  6.f,  7.f,  8.f,
        9.f,  10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
    };
    const auto expected = pbrt::math::Matrix4x4{
        1.f, 5.f, 9.f, 13.f,
        2.f, 6.f, 10.f, 14.f,
        3.f, 7.f, 11.f, 15.f,
        4.f, 8.f, 12.f, 16.f
    };
    // clang-format on

    CHECK(transpose(m) == expected);
}