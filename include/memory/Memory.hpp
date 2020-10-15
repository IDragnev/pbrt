#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#if __has_include(<alloca.h>)
    #include <alloca.h>
#endif
#if __has_include(<malloc.h>)
    #include <malloc.h>
#endif

namespace idragnev::pbrt {
    namespace constants {
#ifndef PBRT_L1_CACHE_LINE_SIZE
        inline constexpr std::size_t L1_CACHE_LINE_SIZE = 64u;
#else
        inline constexpr std::size_t L1_CACHE_LINE_SIZE =
            PBRT_L1_CACHE_LINE_SIZE;
#endif
    } // namespace constants

    template <typename T>
    [[nodiscard]] inline auto pbrtAlloca(const std::size_t count) {
        return static_cast<T*>(alloca(count * sizeof(T)));
    }

    // Allocates memory aligned to L1 Cache line boundary
    [[nodiscard]] void* allocCacheAligned(const std::size_t size);
    void freeAligned(void* ptr);

    // Allocates memory for `count` Ts aligned to L1 Cache line boundary
    template <typename T>
    [[nodiscard]] inline auto allocCacheAligned(const std::size_t count) {
        return static_cast<T*>(allocCacheAligned(count * sizeof(T)));
    }

    template <typename T>
    inline constexpr bool isPowerOfTwo(const T n) noexcept {
        static_assert(
            std::is_integral_v<T> && !std::is_same_v<T, bool>,
            "Cannot call isPowerOfTwo with non-integral type or bool");
        return n > 0 && ((n & (n - 1)) == 0);
    }

    // Align `num` upwards to alignment `align`.
    // Requires that `align` is a power of two.
    inline constexpr std::size_t alignUp(const std::size_t num,
                                         const std::size_t align) {
        return (num + align - 1) & ~(align - 1);
    }

    inline constexpr std::size_t
    toMultipleOfStrictestAlign(const std::size_t size) noexcept {
        return alignUp(size, alignof(std::max_align_t));
    }
} // namespace idragnev::pbrt