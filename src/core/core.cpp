#include "core/core.hpp"

namespace idragnev::pbrt {
    float nextFloatUp(float f) noexcept {
        if (std::isinf(f) && f > 0.f) {
            return f;
        }

        if (f == -0.f) {
            f = 0.f;
        }

        auto bits = floatToBits(f);
        if (f >= 0.f) {
            ++bits;
        }
        else {
            --bits;
        }

        return bitsToFloat(bits);
    }

    float nextFloatDown(float v) noexcept {
        if (std::isinf(v) && v < 0.) {
            return v;
        }

        if (v == 0.f) {
            v = -0.f;
        }

        auto bits = floatToBits(v);
        if (v > 0.f) {
            --bits;
        }
        else {
            ++bits;
        }

        return bitsToFloat(bits);
    }

    double nextFloatUp(double v, const int delta) noexcept {
        if (std::isinf(v) && v > 0.) {
            return v;
        }

        if (v == -0.) {
            v = 0.;
        }

        auto bits = floatToBits(v);
        if (v >= 0.) {
            bits += delta;
        }
        else {
            bits -= delta;
        }

        return bitsToFloat(bits);
    }

    double nextFloatDown(double v, const int delta) noexcept {
        if (std::isinf(v) && v < 0.) {
            return v;
        }

        if (v == 0.) {
            v = -0.;
        }

        auto bits = floatToBits(v);
        if (v > 0.) {
            bits -= delta;
        }
        else {
            bits += delta;
        }

        return bitsToFloat(bits);
    }
} // namespace idragnev::pbrt