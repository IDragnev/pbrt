#include "doctest/doctest.h"
#include "core/Interval.hpp"

namespace pbrt = idragnev::pbrt;

using Interval = pbrt::Interval<int>;

TEST_CASE("construction from two values does not depend on"
          "arguments order") {
    const auto i1 = Interval{1, 0};
    const auto i2 = Interval{0, 1};

    CHECK(i1.low() == 0);
    CHECK(i1.high() == 1);

    CHECK(i2.low() == 0);
    CHECK(i2.high() == 1);
}

TEST_CASE("multiplication") {
    const auto a = Interval{0, 5};
    const auto b = Interval{2, 10};

    const auto c = a * b;

    CHECK(c.low() == 0);
    CHECK(c.high() == 50);
}

TEST_CASE("sum") {
    const auto a = Interval{0, 5};
    const auto b = Interval{2, 10};

    const auto c = a + b;

    CHECK(c.low() == 2);
    CHECK(c.high() == 15);
}

TEST_CASE("subtraction") {
    const auto a = Interval{0, 5};
    const auto b = Interval{2, 10};

    const auto c = a - b;

    CHECK(c.low() == -10);
    CHECK(c.high() == 3);
}

TEST_CASE("negation") {
    const auto a = Interval{2, 10};

    const auto b = -a;

    CHECK(b.low() == -10);
    CHECK(b.high() == -2);
}

TEST_CASE("withLow") {
    const auto interval = Interval{0, 3};

    SUBCASE("with higher argument than high") {
        const auto i = interval.withLow(4);

        CHECK(i.low() == 3);
        CHECK(i.high() == 4);
    }

    SUBCASE("with lower argument than high") {
        const auto i = interval.withLow(1);

        CHECK(i.low() == 1);
        CHECK(i.high() == 3);
    }
}

TEST_CASE("withHigh") {
    const auto interval = Interval{0, 3};

    SUBCASE("with higher argument than high") {
        const auto i = interval.withHigh(4);

        CHECK(i.low() == 0);
        CHECK(i.high() == 4);
    }

    SUBCASE("with lower argument than high") {
        const auto i = interval.withHigh(1);

        CHECK(i.low() == 0);
        CHECK(i.high() == 1);
    }
}