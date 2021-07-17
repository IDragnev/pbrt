#pragma once

#include "Fwd.hpp"
#include "pbrt/core/core.hpp"

#include <concepts>

namespace idragnev::pbrt::math {
    namespace constants {
        // clang-format off
        inline constexpr auto Pi = static_cast<Float>(3.14159265358979323846);
        inline constexpr auto InvPi = static_cast<Float>(0.31830988618379067154);
        inline constexpr auto Inv2Pi = static_cast<Float>(0.15915494309189533577);
        inline constexpr auto Inv4Pi = static_cast<Float>(0.07957747154594766788);
        inline constexpr auto PiOver2 = static_cast<Float>(1.57079632679489661923);
        inline constexpr auto PiOver4 = static_cast<Float>(0.78539816339744830961);
        inline constexpr auto Sqrt2 = static_cast<Float>(1.41421356237309504880);
        // clang-format on
    } // namespace constants

    inline Float toRadians(const Float deg) noexcept {
        using constants::Pi;
        return (Pi / 180.f) * deg;
    }

    inline Float toDegrees(const Float rad) noexcept {
        using constants::Pi;
        return (180.f / Pi) * rad;
    }

    template <std::integral T>
    inline T mod(T a, T m) {
        const T result = a - (a / m) * m;
        return static_cast<T>((result < 0) ? result + m : result);
    }

    template <std::floating_point T>
    inline T mod(T a, T m) {
        return std::fmod(a, m);
    }

    void extendedGCD(std::uint64_t a,
                     std::uint64_t b,
                     std::int64_t& x,
                     std::int64_t& y);
    // The multiplicative inverse of `a` mod `m`, where
    // `a` and `m` are coprime.
    std::uint64_t multInverseCoprimes(const std::int64_t a,
                                      const std::int64_t m);

    Intervalf sin(const Intervalf& i);
    Intervalf cos(const Intervalf& i);
} // namespace idragnev::pbrt::math