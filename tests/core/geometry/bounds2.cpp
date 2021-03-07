#include "doctest/doctest.h"
#include "pbrt/core/geometry/Bounds2.hpp"

#include <vector>

namespace pbrt = idragnev::pbrt;

TEST_CASE("iterating through Bounds2i") {
    const auto collect = [](const pbrt::Bounds2i& bounds) {
        std::vector<pbrt::Point2i> result;

        for (const auto& p : bounds) {
            result.push_back(p);
        }

        return result;
    };

    SUBCASE("empty bounds") {
        const auto bounds = pbrt::Bounds2i{{0, 0}, {0, 0}};

        const auto points = collect(bounds);

        CHECK(points.empty());
    }

    SUBCASE("non-empty bounds") {
        const auto bounds = pbrt::Bounds2i{{0, 0}, {2, 2}};
        const auto expected =
            std::vector<pbrt::Point2i>{{0, 0}, {1, 0}, {0, 1}, {1, 1}};

        const auto points = collect(bounds);

        CHECK(points == expected);
    }
}

TEST_CASE("inside") {
    const auto bounds = pbrt::Bounds2i{{0, 0}, {3, 3}};

    SUBCASE("ends are inside") {
        CHECK(inside(bounds.min, bounds));
        CHECK(inside(bounds.max, bounds));
    }

    SUBCASE("point outside of bounds") {
        const auto p = 2 * bounds.max;
        CHECK(!inside(p, bounds));
    }

    SUBCASE("point inside of bounds") {
        const auto p = (bounds.min + bounds.max) / 2;
        CHECK(inside(p, bounds));
    }
}

TEST_CASE("unionOf") {
    SUBCASE("same bounds") {
        const auto b = pbrt::Bounds2i{{0, 0}, {3, 3}};

        CHECK(pbrt::unionOf(b, b) == b);
    }

    SUBCASE("different bounds") {
        const auto bounds = pbrt::unionOf(pbrt::Bounds2i{{0, 0}, {3, 3}},
                                          pbrt::Bounds2i{{1, 1}, {4, 4}});

        CHECK(bounds == pbrt::Bounds2i{{0, 0}, {4, 4}});
    }

    SUBCASE("bounds with point inside it") {
        const auto b = pbrt::Bounds2i{{0, 0}, {3, 3}};

        CHECK(pbrt::unionOf(b, b.min) == b);
        CHECK(pbrt::unionOf(b, b.max) == b);
    }

    SUBCASE("bounds with point outside of it") {
        const auto b = pbrt::Bounds2i{{0, 0}, {3, 3}};

        CHECK(pbrt::unionOf(b, 2 * b.min) == b);
        CHECK(pbrt::unionOf(b, b.max / 2) == b);
    }
}

TEST_CASE("intersectionOf") {
    SUBCASE("same bounds") {
        const auto b = pbrt::Bounds2i{{0, 0}, {3, 3}};

        CHECK(pbrt::intersectionOf(b, b) == b);
    }

    SUBCASE("different bounds") {
        const auto bounds =
            pbrt::intersectionOf(pbrt::Bounds2i{{0, 0}, {3, 3}},
                                 pbrt::Bounds2i{{1, 1}, {4, 4}});

        CHECK(bounds == pbrt::Bounds2i{{1, 1}, {3, 3}});
    }
}

TEST_CASE("overlap") {
    SUBCASE("same bounds") {
        const auto b = pbrt::Bounds2i{{0, 0}, {3, 3}};

        CHECK(pbrt::overlap(b, b));
    }

    SUBCASE("overlapping bounds") {
        const auto a = pbrt::Bounds2i{{0, 0}, {3, 3}};
        const auto b = pbrt::Bounds2i{{1, 1}, {4, 4}};

        CHECK(pbrt::overlap(a, b));
    }

    SUBCASE("non-overlapping bounds") {
        const auto a = pbrt::Bounds2i{{0, 0}, {3, 3}};
        const auto b = pbrt::Bounds2i{2 * a.max, 6 * a.max};

        CHECK(!pbrt::overlap(a, b));
    }
}