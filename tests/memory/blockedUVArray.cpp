#include "doctest/doctest.h"
#include "memory/BlockedUVArray.hpp"

#include <algorithm>

namespace pbrt = idragnev::pbrt;

TEST_CASE("zero initialization is used by default") {
    const std::size_t uExtent = 2;
    const std::size_t vExtent = 2;

    pbrt::BlockedUVArray<int, 1> arr(uExtent, vExtent);

    REQUIRE(arr.uExtent() == uExtent);
    REQUIRE(arr.vExtent() == vExtent);

    for (std::size_t v = 0; v < vExtent; ++v) {
        for (std::size_t u = 0; u < uExtent; ++u) {
            CHECK(arr.at(u, v) == 0);
        }
    }
}

#include <iostream>

TEST_CASE("at") {
    const int data[] = {1, 2, 3, 4};
    const std::size_t uExtent = 2;
    const std::size_t vExtent = 2;

    pbrt::BlockedUVArray<int, 1> arr(uExtent, vExtent, data);

    for (std::size_t v = 0; v < vExtent; ++v) {
        for (std::size_t u = 0; u < uExtent; ++u) {
            CHECK(arr.at(u, v) == data[uExtent * v + u]);
        }
    }
}

TEST_CASE("asLinearArray") {
    const int data[] = {1, 2, 3, 4};
    const std::size_t uExtent = 2;
    const std::size_t vExtent = 2;

    pbrt::BlockedUVArray<int, 1> arr(uExtent, vExtent, data);

    const auto size = std::extent_v<decltype(data)>;
    int dest[size] = {};
    arr.asLinearArray(dest);

    CHECK(std::equal(data, data + size, dest, dest + size));
}