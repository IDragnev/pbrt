#include "doctest/doctest.h"
#include "core/Interval.hpp"

namespace pbrt = idragnev::pbrt;

TEST_CASE("construction from two values does not depend on"
          "arguments order") {
    const auto i1 = pbrt::Intervalf{1.f, 0.f};
    const auto i2 = pbrt::Intervalf{0.f, 1.f};

    CHECK(i1.low() == 0.f);
    CHECK(i1.high() == 1.f);

    CHECK(i2.low() == 0.f);
    CHECK(i2.high() == 1.f);
}

TEST_CASE("multiplication") {
    const auto a = pbrt::Intervalf{0.f, 5.f};
    const auto b = pbrt::Intervalf{2.f, 10.f};

    const auto c = a * b;

    CHECK(c.low() == 0.f);
    CHECK(c.high() == 50.f);
}

TEST_CASE("sum") {
    const auto a = pbrt::Intervalf{0.f, 5.f};
    const auto b = pbrt::Intervalf{2.f, 10.f};

    const auto c = a + b;

    CHECK(c.low() == 2.f);
    CHECK(c.high() == 15.f);
}

TEST_CASE("subtraction") {
    const auto a = pbrt::Intervalf{0.f, 5.f};
    const auto b = pbrt::Intervalf{2.f, 10.f};

    const auto c = a - b;

    CHECK(c.low() == -10.f);
    CHECK(c.high() == 3.f);
}

TEST_CASE("negation") {
    const auto a = pbrt::Intervalf{2.f, 10.f};

    const auto b = -a;

    CHECK(b.low() == -10.f);
    CHECK(b.high() == -2.f);
}

TEST_CASE("withLow") {
    const auto interval = pbrt::Intervalf{0.f, 3.f};

    SUBCASE("with higher argument than high") {
        const auto i = interval.withLow(4.f);

        CHECK(i.low() == 3.f);
        CHECK(i.high() == 4.f);
    }

    SUBCASE("with lower argument than high") {
        const auto i = interval.withLow(1.f);

        CHECK(i.low() == 1.f);
        CHECK(i.high() == 3.f);
    }
}

TEST_CASE("withHigh") {
    const auto interval = pbrt::Intervalf{0.f, 3.f};

    SUBCASE("with higher argument than high") {
        const auto i = interval.withHigh(4.f);

        CHECK(i.low() == 0.f);
        CHECK(i.high() == 4.f);
    }

    SUBCASE("with lower argument than high") {
        const auto i = interval.withHigh(1.f);

        CHECK(i.low() == 0.f);
        CHECK(i.high() == 1.f);
    }
}