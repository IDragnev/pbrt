#include "doctest/doctest.h"

#include "pbrt/core/geometry/Bounds3.hpp"
#include "pbrt/core/geometry/Ray.hpp"
#include "pbrt/core/math/Vector3.hpp"

#include <vector>

namespace pbrt = idragnev::pbrt;

TEST_CASE("inside") {
    const auto bounds = pbrt::Bounds3<int>{{0, 0, 0}, {3, 3, 3}};

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
        const auto b = pbrt::Bounds3<int>{{0, 0, 0}, {3, 3, 3}};

        CHECK(pbrt::unionOf(b, b) == b);
    }

    SUBCASE("different bounds") {
        const auto bounds =
            pbrt::unionOf(pbrt::Bounds3<int>{{0, 0, 0}, {3, 3, 3}},
                          pbrt::Bounds3<int>{{1, 1, 1}, {4, 4, 4}});

        CHECK(bounds == pbrt::Bounds3<int>{{0, 0, 0}, {4, 4, 4}});
    }

    SUBCASE("bounds with point inside it") {
        const auto b = pbrt::Bounds3<int>{{0, 0, 0}, {3, 3, 3}};

        CHECK(pbrt::unionOf(b, b.min) == b);
        CHECK(pbrt::unionOf(b, b.max) == b);
    }

    SUBCASE("bounds with point outside of it") {
        const auto b = pbrt::Bounds3<int>{{0, 0, 0}, {3, 3, 3}};

        CHECK(pbrt::unionOf(b, 2 * b.min) == b);
        CHECK(pbrt::unionOf(b, b.max / 2) == b);
    }
}

TEST_CASE("intersectionOf") {
    SUBCASE("same bounds") {
        const auto b = pbrt::Bounds3<int>{{0, 0, 0}, {3, 3, 3}};

        CHECK(pbrt::intersectionOf(b, b) == b);
    }

    SUBCASE("different bounds") {
        const auto bounds =
            pbrt::intersectionOf(pbrt::Bounds3<int>{{0, 0, 0}, {3, 3, 3}},
                                 pbrt::Bounds3<int>{{1, 1, 1}, {4, 4, 4}});

        CHECK(bounds == pbrt::Bounds3<int>{{1, 1, 1}, {3, 3, 3}});
    }
}

TEST_CASE("overlap") {
    SUBCASE("same bounds") {
        const auto b = pbrt::Bounds3<int>{{0, 0, 0}, {3, 3, 3}};

        CHECK(pbrt::overlap(b, b));
    }

    SUBCASE("overlapping bounds") {
        const auto a = pbrt::Bounds3<int>{{0, 0, 0}, {3, 3, 3}};
        const auto b = pbrt::Bounds3<int>{{1, 1, 1}, {4, 4, 4}};

        CHECK(pbrt::overlap(a, b));
    }

    SUBCASE("non-overlapping bounds") {
        const auto a = pbrt::Bounds3<int>{{0, 0, 0}, {3, 3, 3}};
        const auto b = pbrt::Bounds3<int>{2 * a.max, 6 * a.max};

        CHECK(!pbrt::overlap(a, b));
    }
}

TEST_CASE("point distance") {
    SUBCASE("with points inside the bounds or on faces") {
        const auto bounds = pbrt::Bounds3f{{0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}};

        CHECK(distance(pbrt::Point3f{0.5f, 0.5f, 0.5f}, bounds) == 0.f);
        CHECK(distance(pbrt::Point3f{0.f, 1.f, 1.f}, bounds) == 0.f);
        CHECK(distance(pbrt::Point3f{0.25f, 0.8f, 1.f}, bounds) == 0.f);
        CHECK(distance(pbrt::Point3f{0.f, 0.25f, 0.8f}, bounds) == 0.f);
        CHECK(distance(pbrt::Point3f{0.7f, 0.f, 0.8f}, bounds) == 0.f);
    }

    SUBCASE("with points aligned with the plane of one of the faces") {
        const auto bounds = pbrt::Bounds3f{{0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}};

        CHECK(pbrt::distance(pbrt::Point3f{6.f, 1.f, 1.f}, bounds) == 5.f);
        CHECK(pbrt::distance(pbrt::Point3f{0.f, -10.f, 1.f}, bounds) == 10.f);
    }

    SUBCASE("with 2 of the point's the dimensions inside the bounds") {
        const auto bounds = pbrt::Bounds3f{{0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}};

        CHECK(distance(pbrt::Point3f{0.5f, 0.5f, 3.f}, bounds) == 2.f);
        CHECK(distance(pbrt::Point3f{0.5f, 0.5f, -3.f}, bounds) == 3.f);
        CHECK(distance(pbrt::Point3f{0.5f, 3, 0.5f}, bounds) == 2.f);
        CHECK(distance(pbrt::Point3f{0.5f, -3, 0.5f}, bounds) == 3.f);
        CHECK(distance(pbrt::Point3f{3.f, 0.5f, 0.5f}, bounds) == 2.f);
        CHECK(distance(pbrt::Point3f{-3.f, 0.5f, 0.5f}, bounds) == 3.f);
    }

    SUBCASE("with general points") {
        const auto bounds = pbrt::Bounds3f{{0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}};

        CHECK(distanceSquared(pbrt::Point3f{4.f, 8.f, -10.f}, bounds) ==
              3 * 3 + 7 * 7 + 10 * 10);
        CHECK(distanceSquared(pbrt::Point3f{-6.f, -10.f, 8.f}, bounds) ==
              6 * 6 + 10 * 10 + 7 * 7);
    }

    SUBCASE("with a more irregular box") {
        const auto bounds =
            pbrt::Bounds3f{{-1.f, -3.f, 5.f}, {2.f, -2.f, 18.f}};

        CHECK(distance(pbrt::Point3f{-0.99f, -2.f, 5.f}, bounds) == 0.f);
        CHECK(distanceSquared(pbrt::Point3f{-3.f, -9.f, 22.f}, bounds) ==
              2 * 2 + 6 * 6 + 4 * 4);
    }
}

TEST_CASE("corner selection") {
    const auto bounds = pbrt::Bounds3f{{0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}};

    CHECK(bounds.corner(0) == bounds.min);
    CHECK(bounds.corner(1) == pbrt::Point3f{1.f, 0.f, 0.f});
    CHECK(bounds.corner(2) == pbrt::Point3f{0.f, 1.f, 0.f});
    CHECK(bounds.corner(3) == pbrt::Point3f{1.f, 1.f, 0.f});
    CHECK(bounds.corner(4) == pbrt::Point3f{0.f, 0.f, 1.f});
    CHECK(bounds.corner(5) == pbrt::Point3f{1.f, 0.f, 1.f});
    CHECK(bounds.corner(6) == pbrt::Point3f{0.f, 1.f, 1.f});
    CHECK(bounds.corner(7) == bounds.max);
}

TEST_CASE("ray outside the bounds does not intersect it") {
    const auto bounds = pbrt::Bounds3f{
        pbrt::Point3f{1.0, 1.0, 1.0},
        pbrt::Point3f{5.0, 10.0, 30.0},
    };
    const auto ray =
        pbrt::Ray{pbrt::Point3f{0.f, 0.f, 0.f}, pbrt::Vector3f{-1.f, 1.f, 1.f}};

    CHECK(bounds.intersectP(ray) == pbrt::nullopt);
}

TEST_CASE("ray parallel to the bounds does not intersect it") {
    const auto bounds = pbrt::Bounds3f{
        pbrt::Point3f{1.f, 1.f, 1.f},
        pbrt::Point3f{5.f, 10.f, 30.f},
    };
    const auto ray = pbrt::Ray{
        pbrt::Point3f{0.f, 0.f, 0.f},
        pbrt::Vector3f{0.f, 1.f, 1.f},
    };

    CHECK(bounds.intersectP(ray) == pbrt::nullopt);
}

TEST_CASE("ray through the bounds intersects it") {
    const auto bounds = pbrt::Bounds3f{
        pbrt::Point3f{1.f, 1.f, 1.f},
        pbrt::Point3f{5.f, 10.f, 30.f},
    };
    const auto ray = pbrt::Ray{
        pbrt::Point3f{0.f, 0.f, 0.f},
        pbrt::Vector3f{1.f, 1.f, 1.f},
    };

    CHECK(bounds.intersectP(ray) != pbrt::nullopt);
}

TEST_CASE("ray with origin inside the bounds intersects it") {
    const auto bounds = pbrt::Bounds3f{
        pbrt::Point3f{1.f, 1.f, 1.f},
        pbrt::Point3f{3.f, 3.f, 3.f},
    };
    const auto ray = pbrt::Ray{
        pbrt::Point3f{2.f, 2.f, 2.f},
        pbrt::Vector3f{0.f, 1.f, 0.f},
    };

    CHECK(bounds.intersectP(ray) != pbrt::nullopt);
}

TEST_CASE("ray with origin on a bounds wall and passing through the bounds "
          "intersects it") {
    const auto bounds = pbrt::Bounds3f{
        pbrt::Point3f{1.f, 1.f, 1.f},
        pbrt::Point3f{3.f, 3.f, 3.f},
    };
    const auto ray = pbrt::Ray{
        pbrt::Point3f{1.f, 1.f, 2.f},
        pbrt::Vector3f{0.f, 1.f, 0.f},
    };

    CHECK(bounds.intersectP(ray) != pbrt::nullopt);
}

TEST_CASE("optimised intersection") {
    const auto inverse = [](const auto vec) {
        return pbrt::Vector3f{1 / vec[0], 1 / vec[1], 1 / vec[2]};
    };
    const std::size_t dirIsNegative[] = {0, 0, 0};

    SUBCASE("ray outside the bounds does not intersect it") {
        const auto bounds = pbrt::Bounds3f{
            pbrt::Point3f{1.f, 1.f, 1.f},
            pbrt::Point3f{5.f, 10.f, 30.f},
        };
        const auto ray = pbrt::Ray{pbrt::Point3f{0.f, 0.f, 0.f},
                                   pbrt::Vector3f{-1.f, 1.f, 1.f}};

        CHECK(!bounds.intersectP(ray, inverse(ray.d), dirIsNegative));
    }

    SUBCASE("ray parallel to the bounds does not intersect it") {
        const auto bounds = pbrt::Bounds3f{
            pbrt::Point3f{1.f, 1.f, 1.f},
            pbrt::Point3f{5.f, 10.f, 30.f},
        };
        const auto ray = pbrt::Ray{
            pbrt::Point3f{0.f, 0.f, 0.f},
            pbrt::Vector3f{0.f, 1.f, 1.f},
        };

        CHECK(!bounds.intersectP(ray, inverse(ray.d), dirIsNegative));
    }

    SUBCASE("ray through the bounds intersects it") {
        const auto bounds = pbrt::Bounds3f{
            pbrt::Point3f{1.f, 1.f, 1.f},
            pbrt::Point3f{5.f, 10.f, 30.f},
        };
        const auto ray = pbrt::Ray{
            pbrt::Point3f{0.f, 0.f, 0.f},
            pbrt::Vector3f{1.f, 1.f, 1.f},
        };

        CHECK(bounds.intersectP(ray, inverse(ray.d), dirIsNegative));
    }

    SUBCASE("ray with origin on a bounds wall and passing through the bounds "
            "intersects it") {
        const auto bounds = pbrt::Bounds3f{
            pbrt::Point3f{1.f, 1.f, 1.f},
            pbrt::Point3f{3.f, 3.f, 3.f},
        };
        const auto ray = pbrt::Ray{
            pbrt::Point3f{1.f, 1.f, 2.f},
            pbrt::Vector3f{0.f, 1.f, 0.f},
        };

        CHECK(bounds.intersectP(ray, inverse(ray.d), dirIsNegative));
    }

    SUBCASE("ray with origin inside the bounds intersects it") {
        const auto bounds = pbrt::Bounds3f{
            pbrt::Point3f{1.f, 1.f, 1.f},
            pbrt::Point3f{3.f, 3.f, 3.f},
        };
        const auto ray = pbrt::Ray{
            pbrt::Point3f{2.f, 2.f, 2.f},
            pbrt::Vector3f{0.f, 1.f, 0.f},
        };

        CHECK(bounds.intersectP(ray, inverse(ray.d), dirIsNegative));
    }
}