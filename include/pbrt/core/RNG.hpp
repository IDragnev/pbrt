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

    class RNG
    {
    public:
        RNG();
        RNG(const std::uint64_t sequenceIndex) { setSequence(sequenceIndex); }

        void setSequence(const std::uint64_t sequenceIndex);

        std::uint32_t uniformUInt32();
        std::uint32_t uniformUInt32(const std::uint32_t b);

        Float uniformFloat();

    private:
        std::uint64_t state = 0;
        std::uint64_t inc = 0;
    };
} // namespace idragnev::pbrt::rng