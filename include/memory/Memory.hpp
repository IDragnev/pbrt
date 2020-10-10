#pragma once

#include <cstddef>

#if __has_include(<alloca.h>)
    #include <alloca.h>
#endif
#if __has_include(<malloc.h>)
    #include <malloc.h>
#endif

namespace idragnev::pbrt {
    namespace constants {
        inline constexpr std::size_t L1_CACHE_LINE_SIZE = 64u;
    }

    template <typename T>
    [[nodiscard]] inline auto pbrtAlloca(const std::size_t count) {
        return static_cast<T*>(alloca(count * sizeof(T)));
    }

    [[nodiscard]] void* allocAligned(const std::size_t size);
    void freeAligned(void* ptr);

    template <typename T>
    [[nodiscard]] inline auto allocAligned(const std::size_t count) {
        return static_cast<T*>(allocAligned(count * sizeof(T)));
    }
} // namespace idragnev::pbrt