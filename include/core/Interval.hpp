#pragma once

#include "core.hpp"

namespace idragnev::pbrt {
    class Interval
    {
    public:
        struct NoOrderCheck
        {
        };

        Interval(const Float x) noexcept : _low(x), _high(x) {}
        Interval(const Float a, const Float b) noexcept;
        Interval(const Float a, const Float b, const NoOrderCheck) noexcept
            : _low(a)
            , _high(b) {}

        Float low() const noexcept { return _low; }
        Float high() const noexcept { return _high; }

        Interval withLow(const Float low) const noexcept {
            return Interval{low, _high};
        }
        Interval withHigh(const Float high) const noexcept {
            return Interval{_low, high};
        }

    private:
        Float _low;
        Float _high;
    };

    Interval operator*(const Interval& lhs, const Interval& rhs) noexcept;

    inline Interval operator-(const Interval& i) noexcept {
        return Interval{-i.high(), -i.low(), Interval::NoOrderCheck{}};
    }

    inline Interval operator+(const Interval& lhs,
                              const Interval& rhs) noexcept {
        return Interval{lhs.low() + rhs.low(), lhs.high() + rhs.high()};
    }

    inline Interval operator-(const Interval& lhs,
                              const Interval& rhs) noexcept {
        return Interval{lhs.low() - rhs.high(), lhs.high() - rhs.low()};
    }

    Interval sin(const Interval& i);
    Interval cos(const Interval& i);
} // namespace idragnev::pbrt