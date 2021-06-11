#pragma once

#include "pbrt/core/core.hpp"

namespace idragnev::pbrt::rng {
    namespace constants {
        // clang-format off
        inline static constexpr float FloatOneMinusEpsilon = 0x1.fffffep-1;
        inline static constexpr double DoubleOneMinusEpsilon = 0x1.fffffffffffffp-1;

#ifdef PBRT_FLOAT_AS_DOUBLE
        inline static constexpr Float OneMinusEpsilon = DoubleOneMinusEpsilon;
#else
        inline static constexpr Float OneMinusEpsilon = FloatOneMinusEpsilon;
#endif
        // clang-format on
    } // namespace constants

    // TODO
    class RNG
    {
    public:
        Float uniformFloat() {
            // TODO
            return constants::OneMinusEpsilon;
        }

        std::uint32_t uniformUInt32([[maybe_unused]] const std::uint32_t b) {
            // TODO
            return 0;
        }

    };
} // namespace idragnev::pbrt::rng