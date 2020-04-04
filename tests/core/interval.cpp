#include "doctest/doctest.h"

#include "Interval.hpp"

namespace pbrt = idragnev::pbrt;

TEST_CASE("construction from two values does not depend on"
          "arguments order")
{
    const auto i = pbrt::Interval{1.f, 0.f};

    CHECK(i.low() == 0.f);
    CHECK(i.high() == 1.f);
}

TEST_CASE("multiplication")
{
    const auto a = pbrt::Interval{0.f, 5.f};
    const auto b = pbrt::Interval{2.f, 10.f};
    
    const auto c = a * b;

    CHECK(c.low() == 0.f);
    CHECK(c.high() == 50.f);
}

TEST_CASE("sum")
{
    const auto a = pbrt::Interval{ 0.f, 5.f };
    const auto b = pbrt::Interval{ 2.f, 10.f };

    const auto c = a + b;

    CHECK(c.low() == 2.f);
    CHECK(c.high() == 15.f);
}

TEST_CASE("subtraction")
{
    const auto a = pbrt::Interval{ 0.f, 5.f };
    const auto b = pbrt::Interval{ 2.f, 10.f };

    const auto c = a - b;

    CHECK(c.low() == -10.f);
    CHECK(c.high() == 3.f);
}