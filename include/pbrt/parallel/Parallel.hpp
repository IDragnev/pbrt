#pragma once

#include <functional>
#include <mutex>
#include <condition_variable>

namespace idragnev::pbrt::parallel {
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

    void init();
    void cleanup();

    void parallelFor(std::function<void(std::int64_t)> func,
                     const std::int64_t iterationsCount,
                     const std::int64_t chunkSize = 1);
    void parallelFor2D(std::function<void(std::int64_t, std::int64_t)> func,
                       const std::int64_t nX,
                       const std::int64_t nY);
} // namespace idragnev::pbrt::parallel