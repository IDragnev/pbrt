#pragma once

#include "Memory.hpp"

namespace idragnev::pbrt::memory {
    template <typename T, unsigned LogBlockSize>
    class BlockedUVArray
    {
        static_assert(std::is_default_constructible_v<T>,
                      "T must be default constructible");

    public:
        static inline constexpr std::size_t BLOCK_EXTENT = 1 << LogBlockSize;

        BlockedUVArray(const std::size_t uextent, const std::size_t vextent)
            : BlockedUVArray(uextent, vextent, nullptr) {}
        BlockedUVArray(const std::size_t uextent,
                       const std::size_t vextent,
                       const T* const init);
        ~BlockedUVArray();

        std::size_t uExtent() const noexcept;
        std::size_t vExtent() const noexcept;

        T& at(const std::size_t u, const std::size_t v);
        const T& at(const std::size_t u, const std::size_t v) const;

        void asLinearArray(T* dest) const;

    private:
        static std::size_t allocationSize(const std::size_t uextent,
                                          const std::size_t vextent) noexcept;
        static std::size_t
        toMultipleOfBlockExtent(const std::size_t n) noexcept;
        static std::size_t blockCoordinate(const std::size_t n) noexcept;
        static std::size_t blockElementCoordinate(const std::size_t n) noexcept;

    private:
        T* data = nullptr;
        std::size_t uextent = 0;
        std::size_t vextent = 0;
        std::size_t uBlocksCount = 0;
    };
} // namespace idragnev::pbrt::memory

#include "BlockedUVArrayImpl.hpp"