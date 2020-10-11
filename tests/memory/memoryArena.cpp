#include "doctest/doctest.h"
#include "memory/MemoryArena.hpp"

namespace pbrt = idragnev::pbrt;

TEST_CASE("alloc<T> uses zero initialization by default") {
    pbrt::MemoryArena arena;

    const auto size = 10;

    int* const arr = arena.alloc<int>(size);

    for (std::size_t i = 0; i < size; ++i) {
        CHECK(arr[i] == 0);
    }
}

TEST_CASE("reset does not free the allocated memory "
          "but simply reuses it") {
    pbrt::MemoryArena arena;
    [[maybe_unused]] void* const arr = arena.alloc(10);

    const auto allocSize = arena.totalAllocationSize();
    arena.reset();

    CHECK(arena.totalAllocationSize() == allocSize);
}