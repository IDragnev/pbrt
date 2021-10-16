#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "pbrt/filters/Box.hpp"
#include "pbrt/filters/Triangle.hpp"
#include "pbrt/filters/Gaussian.hpp"
#include "pbrt/filters/Mitchell.hpp"
#include "pbrt/filters/Sinc.hpp"

namespace pbrt = idragnev::pbrt;

static void testFilterDecreasesApproachingRadius(const pbrt::Filter& filter);
static void testFilterEvalsToZeroOutsideExtent(const pbrt::Filter& filter);

TEST_CASE("box") {
    const auto filter = pbrt::filters::BoxFilter(pbrt::Vector2f{1.f, 1.f});

    SUBCASE("eval is constant") {
        CHECK(filter.eval(pbrt::Point2f{0.f, 0.f}) == doctest::Approx(1.f));
        CHECK(filter.eval(pbrt::Point2f{0.5f * filter.radius}) ==
              doctest::Approx(1.f));
    }

    SUBCASE("eval is zero outside the extent") {
        MESSAGE("box does no radius checks so the test warns about this");
        WARN(filter.eval(pbrt::Point2f{2.f * filter.radius}) ==
             doctest::Approx(0.f));
    }
}

TEST_CASE("triangle") {
    const auto filter = pbrt::filters::TriangleFilter(pbrt::Vector2f{2.f, 1.f});
    testFilterDecreasesApproachingRadius(filter);
    testFilterEvalsToZeroOutsideExtent(filter);
}

TEST_CASE("gaussian") {
    const auto filter = pbrt::filters::GaussianFilter(pbrt::Vector2f{1.f, 1.f}, 4.f);
    testFilterDecreasesApproachingRadius(filter);
    testFilterEvalsToZeroOutsideExtent(filter);
}

TEST_CASE("lanczos sinc") {
    const pbrt::Float tau = 3.f;
    const auto filter = pbrt::filters::LanczosSincFilter(pbrt::Vector2f{3.f, 3.f}, tau);

    SUBCASE("evals to 1 at origin"){
        CHECK(filter.eval(pbrt::Point2f{0.f, 0.f}) == doctest::Approx(1.f));
    }

    SUBCASE("decreases between (0, 0) and (+-1, 0)") {
        CHECK_GT(filter.eval(pbrt::Point2f{0.f, 0.f}),
                 filter.eval(pbrt::Point2f{0.5f, 0.f}));
        CHECK_GT(filter.eval(pbrt::Point2f{0.f, 0.f}),
                 filter.eval(pbrt::Point2f{-0.5f, 0.f}));
    }

    SUBCASE("evals to 0 at (+-1, 0) and (+-2, 0)") {
        CHECK(filter.eval(pbrt::Point2f{1.f, 0.f}) == doctest::Approx(0.f));
        CHECK(filter.eval(pbrt::Point2f{-1.f, 0.f}) == doctest::Approx(0.f));
        CHECK(filter.eval(pbrt::Point2f{2.f, 0.f}) == doctest::Approx(0.f));
        CHECK(filter.eval(pbrt::Point2f{-2.f, 0.f}) == doctest::Approx(0.f));
    }

    SUBCASE("eval is negative between (-1, 0) and (-2, 0)") {
        CHECK(filter.eval(pbrt::Point2f{-1.5f, 0.f}) < 0);
        CHECK(filter.eval(pbrt::Point2f{-1.7f, 0.f}) < 0);
    }

    testFilterEvalsToZeroOutsideExtent(filter);
}

TEST_CASE("mitchell") {
    const auto B = pbrt::Float(0.333333);
    const auto C = pbrt::Float(0.333333);
    const auto filter =
        pbrt::filters::MitchellFilter(pbrt::Vector2f{2.f, 2.f}, B, C);

    SUBCASE("eval decreases between (0, 0) and (+-1, 0)") {
        CHECK_GT(filter.eval(pbrt::Point2f{0.f, 0.f}),
                 filter.eval(pbrt::Point2f{0.5f, 0.f}));
        CHECK_GT(filter.eval(pbrt::Point2f{0.f, 0.f}),
                 filter.eval(pbrt::Point2f{-0.5f, 0.f}));
    }

    SUBCASE("eval is zero at radius") {
        CHECK(filter.eval(pbrt::Point2f{2.f, 0.f}) == doctest::Approx(0.f));
        CHECK(filter.eval(pbrt::Point2f{-2.f, 0.f}) == doctest::Approx(0.f));
    }

    SUBCASE("eval is zero outside the extent") {
        MESSAGE("mitchell does no radius checks so the test warns about this");
        WARN(filter.eval(pbrt::Point2f{2.f * filter.radius}) ==
             doctest::Approx(0.f));
    }
}

static void testFilterDecreasesApproachingRadius(const pbrt::Filter& filter) {
    SUBCASE("eval decreases along y") {
        CHECK_GT(filter.eval(pbrt::Point2f{0.f, 0.f * filter.radius.y}),
                 filter.eval(pbrt::Point2f{0.f, 0.3f * filter.radius.y}));
        CHECK_GT(filter.eval(pbrt::Point2f{0.f, 0.3f * filter.radius.y}),
                 filter.eval(pbrt::Point2f{0.f, 0.5f * filter.radius.y}));
        CHECK_GT(filter.eval(pbrt::Point2f{0.f, 0.5f * filter.radius.y}),
                 filter.eval(pbrt::Point2f{0.f, 0.7f * filter.radius.y}));

        CHECK_GT(filter.eval(pbrt::Point2f{0.f, 0.f * filter.radius.y}),
                 filter.eval(pbrt::Point2f{0.f, -0.3f * filter.radius.y}));
        CHECK_GT(filter.eval(pbrt::Point2f{0.f, -0.3f * filter.radius.y}),
                 filter.eval(pbrt::Point2f{0.f, -0.5f * filter.radius.y}));
        CHECK_GT(filter.eval(pbrt::Point2f{0.f, -0.5f * filter.radius.y}),
                 filter.eval(pbrt::Point2f{0.f, -0.7f * filter.radius.y}));
    }

    SUBCASE("eval decreases along x") {
        CHECK_GT(filter.eval(pbrt::Point2f{0.f, 0.f}),
                 filter.eval(pbrt::Point2f{0.3f * filter.radius.x, 0.f}));
        CHECK_GT(filter.eval(pbrt::Point2f{0.3f * filter.radius.x, 0.f}),
                 filter.eval(pbrt::Point2f{0.5f * filter.radius.x, 0.f}));
        CHECK_GT(filter.eval(pbrt::Point2f{0.5f * filter.radius.x, 0.f}),
                 filter.eval(pbrt::Point2f{0.7f * filter.radius.x, 0.f}));

        CHECK_GT(filter.eval(pbrt::Point2f{0.f, 0.f}),
                 filter.eval(pbrt::Point2f{-0.3f * filter.radius.x, 0.f}));
        CHECK_GT(filter.eval(pbrt::Point2f{-0.3f * filter.radius.x, 0.f}),
                 filter.eval(pbrt::Point2f{-0.5f * filter.radius.x, 0.f}));
        CHECK_GT(filter.eval(pbrt::Point2f{-0.5f * filter.radius.x, 0.f}),
                 filter.eval(pbrt::Point2f{-0.7f * filter.radius.x, 0.f}));
    }

}

void testFilterEvalsToZeroOutsideExtent(const pbrt::Filter& filter)
{
    SUBCASE("eval is 0 outside the extent") {
        CHECK(filter.eval(pbrt::Point2f{0.f, filter.radius.y + 0.2f}) ==
              doctest::Approx(0.f));
        CHECK(filter.eval(pbrt::Point2f{filter.radius.x + 0.2f, 0.f}) ==
              doctest::Approx(0.f));
        CHECK(filter.eval(pbrt::Point2f{2.f * filter.radius}) ==
              doctest::Approx(0.f));
    }
}