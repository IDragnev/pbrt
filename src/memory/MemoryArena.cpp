#include "memory/MemoryArena.hpp"

#include <algorithm>

namespace idragnev::pbrt::memory {
    MemoryArena::~MemoryArena() {
        freeAligned(currentBlock.startAddress);
        for (auto& block : usedBlocks) {
            freeAligned(block.startAddress);
        }
        for (auto& block : availableBlocks) {
            freeAligned(block.startAddress);
        }
    }

    // Guarantees that the allocated memory meets the strictest
    // machine alignment (SMA) by:
    // 1. Allocating memory blocks which are cache aligned
    //    (The L1 cache line size meets the SMA)
    // 2. Bumping the allocation size to a multiple of the SMA
    //    so every following allocation is also SMA aligned
    void* MemoryArena::alloc(const std::size_t nBytes) {
        const auto allocSize = toMultipleOfStrictestAlign(nBytes);

        if (currentBlock.usedBytes + allocSize > currentBlock.size) {
            if (currentBlock.startAddress != nullptr) {
                usedBlocks.push_back(currentBlock);
                currentBlock = {};
            }

            const auto it = std::find_if(
                availableBlocks.cbegin(),
                availableBlocks.cend(),
                [allocSize](const Block& b) { return b.size >= allocSize; });
            if (it != availableBlocks.cend()) {
                currentBlock.startAddress = it->startAddress;
                currentBlock.size = it->size;
                availableBlocks.erase(it);
            }

            if (currentBlock.startAddress == nullptr) {
                const auto size = std::max(allocSize, blockSize);
                currentBlock.startAddress =
                    allocCacheAligned<std::uint8_t>(size);
                currentBlock.size = size;
            }

            currentBlock.usedBytes = 0;
        }

        void* memory = currentBlock.startAddress + currentBlock.usedBytes;
        currentBlock.usedBytes += allocSize;

        return memory;
    }

    void MemoryArena::reset() {
        currentBlock.usedBytes = 0;
        availableBlocks.splice(availableBlocks.begin(), std::move(usedBlocks));
    }

    std::size_t MemoryArena::totalAllocationSize() const noexcept {
        std::size_t total = currentBlock.size;
        for (const auto& block : usedBlocks) {
            total += block.size;
        }
        for (const auto& block : availableBlocks) {
            total += block.size;
        }

        return total;
    }
} // namespace idragnev::pbrt::memory