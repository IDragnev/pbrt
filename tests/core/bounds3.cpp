#include "doctest/doctest.h"

#include "Bounds3.hpp"
#include <vector>

namespace pbrt = idragnev::pbrt;

TEST_CASE("inside")
{
    const auto bounds = pbrt::Bounds3<int>{ {0, 0, 0}, {3, 3, 3} };

    SUBCASE("ends are inside")
    {
        CHECK(inside(bounds.min, bounds));
        CHECK(inside(bounds.max, bounds));
    }

    SUBCASE("point outside of bounds")
    {
        const auto p = 2 * bounds.max;
        CHECK(!inside(p, bounds));
    }

    SUBCASE("point inside of bounds")
    {
        const auto p = (bounds.min + bounds.max) / 2;
        CHECK(inside(p, bounds));
    }
}

TEST_CASE("unionOf")
{
    SUBCASE("same bounds")
    {
        const auto b = pbrt::Bounds3<int>{ {0, 0, 0}, {3, 3, 3} };

        CHECK(pbrt::unionOf(b, b) == b);
    }

    SUBCASE("different bounds")
    {
        const auto bounds = pbrt::unionOf(
            pbrt::Bounds3<int>{ {0, 0, 0}, {3, 3, 3} },
            pbrt::Bounds3<int>{ {1, 1, 1}, {4, 4, 4} }
        );

        CHECK(bounds == pbrt::Bounds3<int>{ {0, 0, 0}, {4, 4, 4} });
    }

    SUBCASE("bounds with point inside it")
    {
        const auto b = pbrt::Bounds3<int>{ {0, 0, 0}, {3, 3, 3} };

        CHECK(pbrt::unionOf(b, b.min) == b);
        CHECK(pbrt::unionOf(b, b.max) == b);
    }

    SUBCASE("bounds with point outside of it")
    {
        const auto b = pbrt::Bounds3<int>{ {0, 0, 0}, {3, 3, 3} };

        CHECK(pbrt::unionOf(b, 2 * b.min) == b);
        CHECK(pbrt::unionOf(b, b.max / 2) == b);
    }
}

TEST_CASE("intersectionOf")
{
    SUBCASE("same bounds")
    {
        const auto b = pbrt::Bounds3<int>{ {0, 0, 0}, {3, 3, 3} };

        CHECK(pbrt::intersectionOf(b, b) == b);
    }

    SUBCASE("different bounds")
    {
        const auto bounds = pbrt::intersectionOf(
            pbrt::Bounds3<int>{{0, 0, 0}, {3, 3, 3}},
            pbrt::Bounds3<int>{{1, 1, 1}, {4, 4, 4}}
        );

        CHECK(bounds == pbrt::Bounds3<int>{ {1, 1, 1}, {3, 3, 3}});
    }
}

TEST_CASE("overlap")
{
    SUBCASE("same bounds")
    {
        const auto b = pbrt::Bounds3<int>{ {0, 0, 0}, {3, 3, 3} };

        CHECK(pbrt::overlap(b, b));
    }

    SUBCASE("overlapping bounds")
    {
        const auto a = pbrt::Bounds3<int>{ {0, 0, 0}, {3, 3, 3} };
        const auto b = pbrt::Bounds3<int>{ {1, 1, 1}, {4, 4, 4} };

        CHECK(pbrt::overlap(a, b));
    }

    SUBCASE("non-overlapping bounds")
    {
        const auto a = pbrt::Bounds3<int>{ {0, 0, 0}, {3, 3, 3} };
        const auto b = pbrt::Bounds3<int>{ 2 * a.max, 6 * a.max };

        CHECK(!pbrt::overlap(a, b));
    }
}