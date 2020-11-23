#pragma once

#include "Memory.hpp"

#include <list>

namespace idragnev::pbrt::memory {

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4324) // structure padding due to alignment specifier
#endif
    class alignas(constants::L1_CACHE_LINE_SIZE) MemoryArena
    {
        struct Block
        {
            std::size_t size = 0;
            std::uint8_t* startAddress = nullptr;
        };

        struct CurrentBlock : public Block
        {
            std::size_t usedBytes = 0;
        };

    public:
        MemoryArena() = default;
        MemoryArena(const std::size_t blockSize) : blockSize(blockSize) {}
        ~MemoryArena();

        MemoryArena(const MemoryArena&) = delete;
        MemoryArena& operator=(const MemoryArena&) = delete;

        [[nodiscard]] void* alloc(const std::size_t size);
        template <typename T>
        [[nodiscard]] T* alloc(const std::size_t count = 1,
                               const bool zeroInitialize = true);

        void reset();

        std::size_t totalAllocationSize() const noexcept;

    private:
        std::size_t blockSize = 262144u; // 256 kB
        CurrentBlock currentBlock{};
        std::list<Block> usedBlocks;
        std::list<Block> availableBlocks;
    };
#ifdef _MSC_VER
    #pragma warning(pop)
#endif

    template <typename T>
    T* MemoryArena::alloc(const std::size_t count, const bool zeroInitialize) {
        static_assert(std::is_default_constructible_v<T>,
                      "T must be default constructible");
        auto* const mem = static_cast<T*>(this->alloc(count * sizeof(T)));
        if (zeroInitialize) {
            for (std::size_t i = 0; i < count; ++i) {
                new (&mem[i]) T{};
            }
        }

        return mem;
    }
} // namespace idragnev::pbrt::memory