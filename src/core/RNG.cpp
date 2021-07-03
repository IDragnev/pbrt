#include "pbrt/core/RNG.hpp"

namespace idragnev::pbrt::rng {
    inline constexpr std::uint64_t PCG32_DEFAULT_STATE = 0x853c49e6748fea9bULL;
    inline constexpr std::uint64_t PCG32_DEFAULT_STREAM = 0xda3e39cb94b95bdbULL;
    inline constexpr std::uint64_t PCG32_MULT = 0x5851f42d4c957f2dULL;

    RNG::RNG() : state(PCG32_DEFAULT_STATE), inc(PCG32_DEFAULT_STREAM) {}

    void RNG::setSequence(const std::uint64_t initseq) {
        state = 0u;
        inc = (initseq << 1u) | 1u;

        uniformUInt32();
        state += PCG32_DEFAULT_STATE;
        uniformUInt32();
    }

    std::uint32_t RNG::uniformUInt32() {
        const std::uint64_t oldstate = state;
        state = oldstate * PCG32_MULT + inc;

        const auto xorshifted =
            static_cast<std::uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
        const auto rot = static_cast<std::uint32_t>(oldstate >> 59u);

        return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
    }

    std::uint32_t RNG::uniformUInt32(const std::uint32_t b) {
        const std::uint32_t threshold = (~b + 1u) % b;

        while (true) {
            const auto r = uniformUInt32();
            if (r >= threshold) {
                return r % b;
            }
        }
    }

    Float RNG::uniformFloat() {
        return std::min(constants::OneMinusEpsilon,
                        Float(uniformUInt32() * 2.3283064365386963e-10f));
    }
} // namespace idragnev::pbrt::rng