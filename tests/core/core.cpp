#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "Vector3.hpp"
#include "Normal3.hpp"
#include "EFloat.hpp"

namespace pbrt = idragnev::pbrt;

TEST_CASE("coordinateSystem returns orthonormal basis when given normalized vector")
{
    const auto [u, v, w] = pbrt::coordinateSystem(pbrt::Vector3f{ 1.f, 0.f, 0.f });

    CHECK(u.length() == 1.f);
    CHECK(v.length() == 1.f);
    CHECK(w.length() == 1.f);

    CHECK(dot(u, v) == 0.f);
    CHECK(dot(u, w) == 0.f);
    CHECK(dot(w, v) == 0.f);
}

TEST_CASE("faceforward flips a normal so that it lies in the same hemisphere"
    "as a vector")
{
    const auto v = pbrt::Vector3f{ 1.f, 0.f, 0.f };

    SUBCASE("same hemisphere")
    {
        const auto n = pbrt::Normal3f{ 2.f, 0.f, 0.f };

        const auto flipped = pbrt::faceforward(n, v);

        CHECK(flipped == n);
    }

    SUBCASE("opposite hemisphere")
    {
        const auto n = pbrt::Normal3f{ -2.f, 0.f, 0.f };

        const auto flipped = pbrt::faceforward(n, v);

        CHECK(flipped == -n);
    }
}

TEST_CASE("solve quadratic with no roots")
{
    const auto a = pbrt::EFloat{1.f};
    const auto b = pbrt::EFloat{-1.f};
    const auto c = pbrt::EFloat{1.f};

    const auto result = pbrt::solveQuadratic(a, b, c);

    CHECK(result == std::nullopt);
}

TEST_CASE("solve quadratic with two different roots")
{
    const auto a = pbrt::EFloat{2.f};
    const auto b = pbrt::EFloat{5.f};
    const auto c = pbrt::EFloat{-3.f};

    const auto result = pbrt::solveQuadratic(a, b, c);

    REQUIRE(result.has_value());

    const auto [t0, t1] = result.value();
    CHECK(static_cast<float>(t0) == doctest::Approx(-3.f));
    CHECK(static_cast<float>(t1) == doctest::Approx(0.5f));
}

TEST_CASE("solve quadratic with one root")
{
    const auto a = pbrt::EFloat{1.f};
    const auto b = pbrt::EFloat{2.f};
    const auto c = pbrt::EFloat{1.f};

    const auto result = pbrt::solveQuadratic(a, b, c);

    REQUIRE(result.has_value());

    const auto [t0, t1] = result.value();

    CHECK(t0 == t1);
    CHECK(static_cast<float>(t0) == doctest::Approx(-1.f));
}