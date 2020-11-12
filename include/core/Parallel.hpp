#pragma once

#include "core.hpp"

#include <atomic>
#include <functional>
#include <mutex>
#include <condition_variable>

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

    // Simple one-use barrier; ensures that multiple threads all reach a
    // particular point of execution before allowing any of them to proceed
    // past it.
    class Barrier
    {
    public:
        Barrier(const int threadsCount);
        ~Barrier();

        void wait();

    private:
        std::mutex mutex;
        std::condition_variable cv;
        int threadsNotReached = 0;
    };

    namespace parallel {
        void init();
        void cleanup();

        void parallelFor(std::function<void(std::int64_t)> func,
                         const std::int64_t iterationsCount,
                         const std::int64_t chunkSize = 1);
        void parallelFor2D(std::function<void(std::int64_t, std::int64_t)> func,
                           const std::int64_t nX,
                           const std::int64_t nY);
    } // namespace parallel
} // namespace idragnev::pbrt