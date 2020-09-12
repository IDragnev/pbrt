#include "doctest/doctest.h"
#include "core/Functional.hpp"

#include <vector>

namespace pbrt = idragnev::pbrt;

TEST_CASE("fmap") {
    const auto square = [](const auto x) {
        return 2 * x;
    };

    SUBCASE("with empty container") {
        const std::vector<int> nums;

        const auto result = pbrt::fmap(nums, square);

        CHECK(result.empty());
    }

    SUBCASE("with non-empty container") {
        const std::vector nums = {1, 2, 3};
        
        const auto result = pbrt::fmap(nums, square);

        CHECK(result == std::vector{2, 4, 6});
    }

    SUBCASE("with prvalue") {
        const auto result = pbrt::fmap(std::vector{1, 2, 3}, square);

        CHECK(result == std::vector{2, 4, 6});
    }

    SUBCASE("with xvalue") {
        std::vector nums = std::vector{1, 2, 3};
        const auto result = pbrt::fmap(std::move(nums), square);

        CHECK(result == std::vector{2, 4, 6});
    }

    SUBCASE("with different inner result type") {
        const std::vector nums = {1};

        const auto result = pbrt::fmap(nums, [](const int) -> std::size_t { return 1; });

        CHECK(result == std::vector<std::size_t>{1});
    }
}