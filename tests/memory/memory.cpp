#include "memory/Memory.hpp"

namespace pbrt = idragnev::pbrt;

static_assert(pbrt::isPowerOfTwo(1));
static_assert(pbrt::isPowerOfTwo(16));
static_assert(pbrt::isPowerOfTwo(32u));
static_assert(pbrt::isPowerOfTwo(64u));
static_assert(!pbrt::isPowerOfTwo(-2));
static_assert(!pbrt::isPowerOfTwo(0));
static_assert(!pbrt::isPowerOfTwo(3));
static_assert(!pbrt::isPowerOfTwo(33u));

static_assert(pbrt::alignUp(2, 2) == 2);
static_assert(pbrt::alignUp(4, 2) == 4);
static_assert(pbrt::alignUp(7, 8) == 8);
static_assert(pbrt::alignUp(9, 16) == 16);
static_assert(pbrt::alignUp(21, 16) == 32);