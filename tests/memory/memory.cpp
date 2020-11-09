#include "memory/Memory.hpp"

namespace mem = idragnev::pbrt::memory;

static_assert(mem::isPowerOfTwo(1));
static_assert(mem::isPowerOfTwo(16));
static_assert(mem::isPowerOfTwo(32u));
static_assert(mem::isPowerOfTwo(64u));
static_assert(!mem::isPowerOfTwo(-2));
static_assert(!mem::isPowerOfTwo(0));
static_assert(!mem::isPowerOfTwo(3));
static_assert(!mem::isPowerOfTwo(33u));

static_assert(mem::alignUp(2, 2) == 2);
static_assert(mem::alignUp(4, 2) == 4);
static_assert(mem::alignUp(7, 8) == 8);
static_assert(mem::alignUp(9, 16) == 16);
static_assert(mem::alignUp(21, 16) == 32);