#include "core/Parallel.hpp"

#include <assert.h>

namespace idragnev::pbrt {
    Barrier::Barrier(const int count) : threadsNotReached(count) {
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
} // namespace idragnev::pbrt