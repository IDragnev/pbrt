#pragma once

#include "core.hpp"

#include <algorithm>

namespace idragnev::pbrt {
    struct NoOrderCheck
    {
    };

    template <typename T>
    class Interval
    {
        static_assert(std::is_arithmetic_v<T>,
                      "Cannot instantiate Interval with non-arithmetic type");

    public:
        Interval(const T x) noexcept : _low(x), _high(x) {}
        Interval(const T a, const T b) noexcept
            : _low(std::min(a, b))
            , _high(std::max(a, b)) {}
        Interval(const T a, const T b, const NoOrderCheck) noexcept
            : _low(a)
            , _high(b) {}

        T low() const noexcept { return _low; }
        T high() const noexcept { return _high; }

        Interval withLow(const T low) const noexcept {
            return Interval{low, _high};
        }
        Interval withHigh(const T high) const noexcept {
            return Interval{_low, high};
        }

    private:
        T _low;
        T _high;
    };

    template <typename T>
    Interval<T> operator*(const Interval<T>& lhs,
                          const Interval<T>& rhs) noexcept {
        // clang-format off
        const auto [min, max] = std::minmax({
                lhs.low()  * rhs.low(),
                lhs.low()  * rhs.high(),
                lhs.high() * rhs.low(),
                lhs.high() * rhs.high()
        });
        // clang-format on

        return Interval<T>{min, max, NoOrderCheck{}};
    }

    template <typename T>
    inline Interval<T> operator-(const Interval<T>& i) noexcept {
        return Interval<T>{-i.high(), -i.low(), NoOrderCheck{}};
    }

    template <typename T>
    inline Interval<T> operator+(const Interval<T>& lhs,
                                 const Interval<T>& rhs) noexcept {
        return Interval<T>{lhs.low() + rhs.low(),
                           lhs.high() + rhs.high(),
                           NoOrderCheck{}};
    }

    template <typename T>
    inline Interval<T> operator-(const Interval<T>& lhs,
                                 const Interval<T>& rhs) noexcept {
        return Interval<T>{lhs.low() - rhs.high(),
                           lhs.high() - rhs.low(),
                           NoOrderCheck{}};
    }

    Intervalf sin(const Intervalf& i);
    Intervalf cos(const Intervalf& i);
} // namespace idragnev::pbrt