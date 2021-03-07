#pragma once

#include "Fwd.hpp"
#include "pbrt/core/core.hpp"

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

    Intervalf sin(const Intervalf& i);
    Intervalf cos(const Intervalf& i);
} // namespace idragnev::pbrt::math