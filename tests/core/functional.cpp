#include "doctest/doctest.h"
#include "core/Functional.hpp"

#include <vector>
#include <string>

namespace pbrt = idragnev::pbrt;

TEST_CASE("fmap") {
    const auto square = [](const auto x) {
        return x * x;
    };

    SUBCASE("on empty container") {
        const std::vector<int> nums;

        const auto result = pbrt::fmap(nums, square);

        CHECK(result.empty());
    }

    SUBCASE("on non-empty container") {
        const std::vector nums = {1, 2, 3};

        const auto result = pbrt::fmap(nums, square);

        CHECK(result == std::vector{1, 4, 9});
    }

    SUBCASE("on prvalue container calls the supplied function with moved values") {
        const auto times2 = [](int&& x) {
            return 2 * x;
        };
        const auto result = pbrt::fmap(std::vector{1, 2, 3}, times2);

        CHECK(result == std::vector{2, 4, 6});
    }

    SUBCASE("on xvalue container calls the supplied function with moved values") {
        const auto move = [](std::string&& s) -> std::string {
            return std::move(s);
        };
        std::vector<std::string> vec = {"a", "b"};

        const auto result = pbrt::fmap(std::move(vec), move);

        CHECK(vec == std::vector<std::string>{"", ""});
        CHECK(result == std::vector<std::string>{"a", "b"});
    }

    SUBCASE("with different inner result type") {
        const std::vector nums = {1};

        const auto result =
            pbrt::fmap(nums, [](const int) -> std::size_t { return 1; });

        CHECK(result == std::vector<std::size_t>{1});
    }
}

TEST_CASE("mapIntegerRange") {
    const auto square = [](const auto x) {
        return x * x;
    };

    SUBCASE("with empty range") {
        const auto result = pbrt::mapIntegerRange<std::vector>(1, 0, square);

        CHECK(result.empty());
    }

    SUBCASE("with non-empty range") {
        const auto result = pbrt::mapIntegerRange<std::vector>(0, 3, square);

        CHECK(result == std::vector{0, 1, 4});
    }
}