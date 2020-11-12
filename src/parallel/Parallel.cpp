#include "parallel/Parallel.hpp"
#include "functional/Functional.hpp"

#include <thread>
#include <assert.h>

namespace idragnev::pbrt::parallel {
    Barrier::Barrier(const int threadsCount) : threadsNotReached(threadsCount) {
        assert(threadsNotReached > 0);
    }

    Barrier::~Barrier() { assert(threadsNotReached == 0); }

    void Barrier::wait() {
        auto lock = std::unique_lock{mutex};
        assert(threadsNotReached > 0);

        threadsNotReached -= 1;
        if (threadsNotReached == 0) {
            cv.notify_all();
        }
        else {
            cv.wait(lock, [this] { return threadsNotReached == 0; });
        }
    }

    class ParallelForLoop;
    struct IterationsChunk;

    namespace statics {
        static ParallelForLoop* workListHead = nullptr;
        static std::mutex workListMutex;
        static std::condition_variable workListCondVar;

        static std::vector<std::thread> threads;
        static bool shutdownThreads = false;

        thread_local int thisThreadIndex;
    } // namespace statics

    int numberOfSystemCores() noexcept;
    int maxThreadIndex() noexcept;

    void workerThread(const int threadIndex);
    void executeChunk(std::unique_lock<std::mutex>& lock,
                      ParallelForLoop& loop,
                      const IterationsChunk& chunk);

    // an exclusive range of loop iterations: [first, last)
    struct IterationsChunk
    {
        std::int64_t first = 0;
        std::int64_t last = 0;
    };

    class ParallelForLoop
    {
    public:
        ParallelForLoop(std::function<void(std::int64_t)> f,
                        const std::int64_t lastIteration,
                        const std::int64_t chunkSize)
            : lastIteration(lastIteration)
            , chunkSize(chunkSize)
            , func1D(std::move(f)) {}

        ParallelForLoop(std::function<void(std::int64_t, std::int64_t)> f,
                        const std::int64_t nX,
                        const std::int64_t nY)
            : lastIteration(nX * nY)
            , nX(nX)
            , func2D(std::move(f)) {}

        bool isFinished() const {
            return hasNoIterationsLeft() && activeWorkers == 0;
        }

        bool hasNoIterationsLeft() const noexcept {
            return nextIteration >= lastIteration;
        }

        IterationsChunk extractNextChunk() {
            const auto first = nextIteration;
            const auto last = std::min(first + chunkSize, lastIteration);

            nextIteration = last;

            return {first, last};
        }

    public:
        ParallelForLoop* next = nullptr;
        std::int64_t lastIteration = 0;
        std::int64_t nextIteration = 0;
        std::int64_t chunkSize = 1;
        std::int64_t nX = -1;
        int activeWorkers = 0;
        std::function<void(std::int64_t)> func1D;
        std::function<void(std::int64_t, std::int64_t)> func2D;
    };

    void init() {
        assert(statics::threads.empty());

        statics::thisThreadIndex = 0;
        const auto threadsCount = maxThreadIndex();

        statics::threads = functional::mapIntegerRange<std::vector>(
            0,
            threadsCount,
            [](const int i) {
                return std::thread{workerThread, i + 1};
            });
    }

    void cleanup() {
        using statics::threads, statics::shutdownThreads;
        using statics::workListCondVar, statics::workListMutex;

        if (threads.empty()) {
            return;
        }

        {
            auto lock = std::lock_guard{workListMutex};
            shutdownThreads = true;
            workListCondVar.notify_all();
        }

        for (std::thread& t : threads) {
            t.join();
        }

        threads.clear();
        shutdownThreads = false;
    }

    void parallelFor(ParallelForLoop& loop);

    void parallelFor(std::function<void(std::int64_t)> func,
                     const std::int64_t iterationsCount,
                     const std::int64_t chunkSize) {
        using statics::threads;

        assert(threads.size() > 0 || maxThreadIndex() == 1);

        if (iterationsCount > chunkSize && threads.size() > 0) {
            ParallelForLoop loop(std::move(func), iterationsCount, chunkSize);
            parallelFor(loop);
        }
        else {
            for (std::int64_t i = 0; i < iterationsCount; ++i) {
                func(i);
            }
        }
    }

    void parallelFor2D(std::function<void(std::int64_t, std::int64_t)> func,
                       const std::int64_t nX,
                       const std::int64_t nY) {
        using statics::threads;

        assert(threads.size() > 0 || maxThreadIndex() == 1);

        if (const auto tilesCount = nX * nY;
            tilesCount > 1 && threads.size() > 0) {
            ParallelForLoop loop(std::move(func), nX, nY);
            parallelFor(loop);
        }
        else {
            for (std::int64_t y = 0; y < nY; ++y) {
                for (std::int64_t x = 0; x < nX; ++x) {
                    func(x, y);
                }
            }
        }
    }

    void parallelFor(ParallelForLoop& loop) {
        using statics::workListHead;
        using statics::workListMutex, statics::workListCondVar;

        {
            const auto lock = std::lock_guard(workListMutex);
            loop.next = workListHead;
            workListHead = &loop;
        }

        auto lock = std::unique_lock(workListMutex);
        workListCondVar.notify_all();

        while (!loop.isFinished()) {
            const auto chunk = loop.extractNextChunk();

            if (loop.hasNoIterationsLeft()) {
                workListHead = loop.next;
            }

            executeChunk(lock, loop, chunk);
        }
    }

    void workerThread(const int threadIndex) {
        using statics::workListCondVar, statics::workListMutex;
        using statics::workListHead, statics::shutdownThreads;

        statics::thisThreadIndex = threadIndex;

        auto lock = std::unique_lock{workListMutex};
        while (!shutdownThreads) {
            if (workListHead == nullptr) {
                workListCondVar.wait(lock);
            }
            else {
                ParallelForLoop& loop = *workListHead;

                const auto chunk = loop.extractNextChunk();

                if (loop.hasNoIterationsLeft()) {
                    workListHead = loop.next;
                }

                executeChunk(lock, loop, chunk);

                if (loop.isFinished()) {
                    workListCondVar.notify_all();
                }
            }
        }
    }

    void executeChunk(std::unique_lock<std::mutex>& lock,
                      ParallelForLoop& loop,
                      const IterationsChunk& chunk) {
        assert(lock.owns_lock());

        loop.activeWorkers += 1;

        // let other threads access the worklist while
        // executing iterations of the current loop
        lock.unlock();
        for (auto i = chunk.first; i < chunk.last; ++i) {
            if (loop.func1D) {
                loop.func1D(i);
            }
            else {
                assert(loop.func2D);
                loop.func2D(i % loop.nX, i / loop.nX);
            }
        }
        lock.lock();

        loop.activeWorkers -= 1;
    }

    int maxThreadIndex() noexcept { return numberOfSystemCores(); }

    int numberOfSystemCores() noexcept {
        return std::max(1u, std::thread::hardware_concurrency());
    }
} // namespace idragnev::pbrt::parallel