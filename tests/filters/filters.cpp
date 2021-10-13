#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "pbrt/filters/Box.hpp"
#include "pbrt/filters/Triangle.hpp"
#include "pbrt/filters/Gaussian.hpp"

namespace pbrt = idragnev::pbrt;

static void testFilter(const pbrt::Filter& filter);

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
    testFilter(filter);
}

TEST_CASE("gaussian") {
    const auto filter = pbrt::filters::GaussianFilter(pbrt::Vector2f{1.f, 1.f}, 4.f);
    testFilter(filter);
}

static void testFilter(const pbrt::Filter& filter) {
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

    SUBCASE("eval is 0 outside the extent") {
        CHECK(filter.eval(pbrt::Point2f{0.f, filter.radius.y + 0.2f}) ==
              doctest::Approx(0.f));
        CHECK(filter.eval(pbrt::Point2f{filter.radius.x + 0.2f, 0.f}) ==
              doctest::Approx(0.f));
        CHECK(filter.eval(pbrt::Point2f{2.f * filter.radius}) ==
              doctest::Approx(0.f));
    }
}