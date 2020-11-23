#pragma once

#include "core.hpp"

#include <atomic>

namespace idragnev::pbrt {
    class AtomicFloat
    {
    private:
#ifdef RT_FLOAT_AS_DOUBLE
        using UnderlyingType = std::uint64_t;
#else
        using UnderlyingType = std::uint32_t;
#endif

    public:
        AtomicFloat() noexcept : AtomicFloat(0.f) {}
        explicit AtomicFloat(const Float f) noexcept : bits(floatToBits(f)) {}

        operator Float() const noexcept { return bitsToFloat(bits); }

        Float operator=(const Float f) noexcept {
            bits = floatToBits(f);
            return f;
        }

        void add(const Float f) noexcept {
            UnderlyingType oldBits = bits;
            UnderlyingType newBits;

            do {
                newBits = floatToBits(bitsToFloat(oldBits) + f);
            } while (!bits.compare_exchange_weak(oldBits, newBits));
        }

    private:
        std::atomic<UnderlyingType> bits;
    };
} // namespace idragnev::pbrt