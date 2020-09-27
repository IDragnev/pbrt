#include "doctest/doctest.h"

#include "core/core.hpp"
#include "core/Parallel.hpp"
#include "core/Functional.hpp"
#include "core/Point2.hpp"

namespace pbrt = idragnev::pbrt;
namespace parallel = pbrt::parallel;

TEST_CASE("cleanup with no init is safe") {
    parallel::cleanup();
}

TEST_CASE("double cleanup is safe") {
    parallel::init();
    parallel::cleanup();
    parallel::cleanup();
}

TEST_CASE("parallelFor basics") {
    [[maybe_unused]] const auto cleanup = pbrt::ScopedFn{[]() noexcept {
        parallel::cleanup();
    }};
    parallel::init();

    SUBCASE("with iterationsCount = 0") {
        std::atomic<int> n = 0;

        parallel::parallelFor([&n](auto) { ++n; }, 0);

        CHECK(n == 0);
    }

    SUBCASE("with iterationsCount > 0") {
        const std::int64_t iterationsCount = 1'000;

        SUBCASE("with chunk size = 1") {
            const std::int64_t chunkSize = 1;

            std::atomic<int> n = 0;

            parallel::parallelFor([&n](auto) { ++n; },
                                  iterationsCount,
                                  chunkSize);

            CHECK(n == iterationsCount);
        }

        SUBCASE("with chunk size > 1") {
            const std::int64_t chunkSize = 20;

            std::atomic<int> n = 0;

            parallel::parallelFor([&n](auto) { ++n; },
                                  iterationsCount,
                                  chunkSize);

            CHECK(n == iterationsCount);
        }
    }
}

TEST_CASE("nested parallelFor") {
    [[maybe_unused]] const auto cleanup = pbrt::ScopedFn{[]() noexcept {
        parallel::cleanup();
    }};
    parallel::init();

    const std::int64_t n = 3;
    int arr[n][n] = {};

    parallel::parallelFor(
        [&arr, n](const auto i) {
            parallel::parallelFor([&arr, i](const auto j) { arr[i][j] = 1; },
                                  n);
        },
        n);

    for (const auto& row : arr) {
        for (const auto e : row) {
            REQUIRE(e == 1);
        }
    }
}

TEST_CASE("parallelFor2D") {
    [[maybe_unused]] const auto cleanup = pbrt::ScopedFn{[]() noexcept {
        parallel::cleanup();
    }};
    parallel::init();

    SUBCASE("with iterationsCount = 0") {
        std::atomic<int> n = 0;

        parallel::parallelFor2D([&n](const pbrt::Point2i) { ++n; },
                                pbrt::Point2i{0, 0});

        CHECK(n == 0);
    }

    SUBCASE("with iterationsCount > 0") {
        const auto iterations = pbrt::Point2i{15, 14};
        std::atomic<int> n = 0;

        parallel::parallelFor2D([&](const pbrt::Point2i) { ++n; },
                                iterations);

        CHECK(n == iterations.x * iterations.y);
    }
}