#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "functional/Functional.hpp"

#include <vector>
#include <string>

namespace funct = idragnev::pbrt::functional;

TEST_CASE("fmap") {
    const auto square = [](const auto x) {
        return x * x;
    };

    SUBCASE("on empty container") {
        const std::vector<int> nums;

        const auto result = funct::fmap(nums, square);

        CHECK(result.empty());
    }

    SUBCASE("on non-empty container") {
        const std::vector nums = {1, 2, 3};

        const auto result = funct::fmap(nums, square);

        CHECK(result == std::vector{1, 4, 9});
    }

    SUBCASE("on prvalue container calls the supplied function with moved values") {
        const auto times2 = [](int&& x) {
            return 2 * x;
        };
        const auto result = funct::fmap(std::vector{1, 2, 3}, times2);

        CHECK(result == std::vector{2, 4, 6});
    }

    SUBCASE("on xvalue container calls the supplied function with moved values") {
        using namespace std::string_literals;

        const auto move = [](std::string&& s) -> std::string {
            return std::move(s);
        };
        std::vector vec = {"a"s, "b"s};

        const auto result = funct::fmap(std::move(vec), move);

        CHECK(vec == std::vector{""s, ""s});
        CHECK(result == std::vector{"a"s, "b"s});
    }

    SUBCASE("with different inner result type") {
        const std::vector nums = {1};

        const auto result =
            funct::fmap(nums, [](const int) -> std::size_t { return 1; });

        CHECK(result == std::vector<std::size_t>{1});
    }
}

TEST_CASE("fmapIndexed") {
    SUBCASE("on empty container") {
        const std::vector<int> nums;

        const auto result =
            funct::fmapIndexed(nums, [](auto, auto) { return 1; });

        CHECK(result.empty());
    }

    SUBCASE("on non-empty container") {
        const std::vector<std::size_t> nums = {1, 2, 3};

        const auto result =
            funct::fmapIndexed(nums, [](std::size_t x, std::size_t i) {
                return x - i;
            });

        CHECK(result == std::vector<std::size_t>{1, 1, 1});
    }

    SUBCASE(
        "on prvalue container calls the supplied function with moved values") {
        const auto result = funct::fmapIndexed(
            std::vector<std::size_t>{1, 2, 3},
            [](std::size_t&& x, std::size_t i) { return x - i; });

        CHECK(result == std::vector<std::size_t>{1, 1, 1});
    }

    SUBCASE(
        "on xvalue container calls the supplied function with moved values") {
        using namespace std::string_literals;

        const auto move = [](std::string&& s, std::size_t) -> std::string {
            return std::move(s);
        };
        std::vector vec = {"a"s, "b"s};

        const auto result = funct::fmapIndexed(std::move(vec), move);

        CHECK(vec == std::vector{""s, ""s});
        CHECK(result == std::vector{"a"s, "b"s});
    }

    SUBCASE("with different inner result type") {
        const std::vector nums = {1};

        const auto result =
            funct::fmapIndexed(nums, [](auto, auto) -> std::size_t {
                return 1;
            });

        CHECK(result == std::vector<std::size_t>{1});
    }
}

TEST_CASE("fmap with IntegerRange") {
    const auto square = [](const auto x) {
        return x * x;
    };

    SUBCASE("with empty range") {
        const auto result =
            funct::fmap<std::vector>(funct::IntegerRange{1, 0}, square);

        CHECK(result.empty());
    }

    SUBCASE("with non-empty range") {
        const auto result =
            funct::fmap<std::vector>(funct::IntegerRange{0, 3}, square);

        CHECK(result == std::vector{0, 1, 4});
    }
}

TEST_CASE("foldl with IntegerRange") {
    SUBCASE("with empty range") {
        const auto n = funct::foldl(funct::IntegerRange{1, 0},
                                    0,
                                    [](auto, auto) { return 10; });
        CHECK(n == 0);
    }

    SUBCASE("with non-empty range") {
        const auto n = funct::foldl(funct::IntegerRange{2, 6},
                                    1,
                                    [](auto acc, auto x) { return acc * x; });
        CHECK(n == 120);
    }
}