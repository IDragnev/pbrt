#include "memory/Memory.hpp"

#ifndef PBRT_HAS_ALIGNED_MALLOC
    #include <stdlib.h>
#endif

namespace idragnev::pbrt::memory {
    static_assert(isPowerOfTwo(alignof(std::max_align_t)),
                  "Strictest machine alignment is not a power of two");
    static_assert(isPowerOfTwo(constants::L1_CACHE_LINE_SIZE),
                  "L1_CACHE_LINE_SIZE must be a power of 2");
    static_assert(
        toMultipleOfStrictestAlign(constants::L1_CACHE_LINE_SIZE) ==
            constants::L1_CACHE_LINE_SIZE,
        "Memory aligned to L1_CACHE_LINE_SIZE will not meet "
        "the strictest machine alignment."
        "L1_CACHE_LINE_SIZE must be a multiple of alignof(std::max_align_t)");

    void* allocCacheAligned(const std::size_t size) {
        using constants::L1_CACHE_LINE_SIZE;

#if defined(PBRT_HAS_ALIGNED_MALLOC)
        return _aligned_malloc(size, L1_CACHE_LINE_SIZE);
#elif defined(PBRT_HAS_POSIX_MEMALIGN)
        void* ptr;
        if (auto r = posix_memalign(&ptr, L1_CACHE_LINE_SIZE, size); r != 0) {
            ptr = nullptr;
        }
        return ptr;
#else // PBRT_HAS_MEMALIGN
        return memalign(L1_CACHE_LINE_SIZE, size);
#endif
    }

    void freeAligned(void* ptr) {
        if (ptr == nullptr) {
            return;
        }

#if defined(PBRT_HAS_ALIGNED_MALLOC)
        _aligned_free(ptr);
#else
        free(ptr);
#endif
    }
} // namespace idragnev::pbrt::memory