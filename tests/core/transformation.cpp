#include "doctest/doctest.h"

#include "core/Transformation.hpp"
#include "core/Bounds3.hpp"

namespace pbrt = idragnev::pbrt;

TEST_CASE("applying translation")
{
    const auto transform = pbrt::translation({ 1.f, 0.f, 0.f });

    SUBCASE("over a vector does nothing")
    {
        const auto v = pbrt::Vector3f{ 0.f, 2.f, 3.f };
 
        CHECK(transform(v) == v);
    }   
    
    SUBCASE("over a point does translate it")
    {
        const auto p = pbrt::Point3f{ 0.f, 2.f, 3.f };

        CHECK(transform(p) == pbrt::Point3f{ 1.f, 2.f, 3.f });
    }

    SUBCASE("over bounds")
    {
        const auto bounds = pbrt::Bounds3f{ {0.f, 0.f, 0.f}, {3.f, 3.f, 3.f} };

        CHECK(transform(bounds) == pbrt::Bounds3f{ {1.f, 0.f, 0.f}, {4.f, 3.f, 3.f} });
    }
}

TEST_CASE("applying scaling")
{
    const auto transform = pbrt::scaling(1.f, 2.f, 3.f);

    SUBCASE("over a vector")
    {
        const auto v = pbrt::Vector3f{ 0.f, 2.f, 3.f };

        CHECK(transform(v) == pbrt::Vector3f{ 0.f, 4.f, 9.f });
    }

    SUBCASE("over a point")
    {
        const auto p = pbrt::Point3f{ 0.f, 2.f, 3.f };

        CHECK(transform(p) == pbrt::Point3f{ 0.f, 4.f, 9.f });
    }

    SUBCASE("over bounds")
    {
        const auto bounds = pbrt::Bounds3f{ {0.f, 0.f, 0.f}, {3.f, 3.f, 3.f} };

        CHECK(transform(bounds) == pbrt::Bounds3f{ {0.f, 0.f, 0.f}, {3.f, 6.f, 9.f} });
    }
}