#include "memory/MemoryArena.hpp"

namespace idragnev::pbrt {
    MemoryArena::~MemoryArena() {
        freeAligned(currentBlock.startAddress);
        for (auto& block : usedBlocks) {
            freeAligned(block.startAddress);
        }
        for (auto& block : availableBlocks) {
            freeAligned(block.startAddress);
        }
    }

    void* MemoryArena::alloc(const std::size_t nBytes) {
        // ensure all handed out memory addresses meet the strictest
        // machine alignment in order to not slow down (or break)
        // memory accesses
        const auto allocSize = toMultipleOfStrictestAlign(nBytes);

        if (currentBlock.usedBytes + allocSize > currentBlock.size) {
            if (currentBlock.startAddress != nullptr) {
                usedBlocks.push_back(currentBlock);
                currentBlock = {};
            }

            for (auto it = availableBlocks.begin(); it != availableBlocks.end();
                 ++it) {
                const auto& block = *it;
                if (block.size >= allocSize) {
                    currentBlock.startAddress = block.startAddress;
                    currentBlock.size = block.size;
                    availableBlocks.erase(it);
                    break;
                }
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
} // namespace idragnev::pbrt