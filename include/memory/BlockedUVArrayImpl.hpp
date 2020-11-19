#include "BlockedUVArray.hpp"

namespace idragnev::pbrt::memory {
    template <typename T, unsigned LogBlockSize>
    BlockedUVArray<T, LogBlockSize>::BlockedUVArray(const std::size_t uextent,
                                                    const std::size_t vextent,
                                                    const T* const init)
        : data([this, uextent, vextent] {
            const auto size = toMultipleOfBlockExtent(uextent) *
                              toMultipleOfBlockExtent(vextent);
            T* const result = allocCacheAligned<T>(size);
            for (std::size_t i = 0; i < size; ++i) {
                new (&result[i]) T{};
            }
            return result;
        }())
        , uextent(uextent)
        , vextent(vextent)
        , uBlocksCount(toMultipleOfBlockExtent(uextent) >> LogBlockSize) {
        if (init != nullptr) {
            for (std::size_t v = 0; v < vextent; ++v) {
                for (std::size_t u = 0; u < uextent; ++u) {
                    this->at(u, v) = init[v * uextent + u];
                }
            }
        }
    }

    template <typename T, unsigned LogBlockSize>
    inline std::size_t BlockedUVArray<T, LogBlockSize>::toMultipleOfBlockExtent(
        const std::size_t n) const noexcept {
        return alignUp(n, BLOCK_EXTENT);
    }

    template <typename T, unsigned LogBlockSize>
    BlockedUVArray<T, LogBlockSize>::~BlockedUVArray() {
        const auto size = uextent * vextent;
        for (std::size_t i = 0; i < size; ++i) {
            data[i].~T();
        }
        freeAligned(data);
    }

    template <typename T, unsigned LogBlockSize>
    inline std::size_t
    BlockedUVArray<T, LogBlockSize>::uExtent() const noexcept {
        return uextent;
    }

    template <typename T, unsigned LogBlockSize>
    inline std::size_t
    BlockedUVArray<T, LogBlockSize>::vExtent() const noexcept {
        return vextent;
    }

    template <typename T, unsigned LogBlockSize>
    void BlockedUVArray<T, LogBlockSize>::asLinearArray(T* dest) const {
        for (std::size_t v = 0; v < vextent; ++v) {
            for (std::size_t u = 0; u < uextent; ++u) {
                *dest = at(u, v);
                ++dest;
            }
        }
    }

    template <typename T, unsigned LogBlockSize>
    inline T& BlockedUVArray<T, LogBlockSize>::at(const std::size_t u,
                                                  const std::size_t v) {
        return const_cast<T&>(
            static_cast<const BlockedUVArray&>(*this).at(u, v));
    }

    template <typename T, unsigned LogBlockSize>
    const T& BlockedUVArray<T, LogBlockSize>::at(const std::size_t u,
                                                 const std::size_t v) const {
        constexpr std::size_t blockSize = BLOCK_EXTENT * BLOCK_EXTENT;
        const std::size_t blockNumber =
            (uBlocksCount * blockCoordinate(v) + blockCoordinate(u));
        const std::size_t blockOffset =
            BLOCK_EXTENT * blockElementCoordinate(v) +
            blockElementCoordinate(u);
        const std::size_t index = blockNumber * blockSize + blockOffset;

        return data[index];
    }

    template <typename T, unsigned LogBlockSize>
    inline std::size_t BlockedUVArray<T, LogBlockSize>::blockCoordinate(
        const std::size_t n) const noexcept {
        return n >> LogBlockSize;
    }

    template <typename T, unsigned LogBlockSize>
    inline std::size_t BlockedUVArray<T, LogBlockSize>::blockElementCoordinate(
        const std::size_t n) const noexcept {
        return (n & (BLOCK_EXTENT - 1));
    }
} // namespace idragnev::pbrt::memory