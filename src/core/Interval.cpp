#include "core/Interval.hpp"

#include <algorithm>
#include <assert.h>

namespace idragnev::pbrt {
    Interval::Interval(const Float a, const Float b) noexcept
        : _low(std::min(a, b))
        , _high(std::max(a, b)) {}

    Interval operator*(const Interval& lhs, const Interval& rhs) noexcept {
        // clang-format off
        const auto [min, max] = std::minmax({
                lhs.low()  * rhs.low(),
                lhs.low()  * rhs.high(),
                lhs.high() * rhs.low(),
                lhs.high() * rhs.high()
        });
        // clang-format on

        return Interval{min, max};
    }

    Interval sin(const Interval& i) {
        using constants::Pi;
        using constants::PiOver2;

        assert(i.low() >= 0.f);
        assert(i.high() <= 2.0001 * Pi);

        auto sinLow = std::sin(i.low());
        auto sinHigh = std::sin(i.high());

        if (sinLow > sinHigh) {
            std::swap(sinLow, sinHigh);
        }
        if (i.low() < PiOver2 && i.high() > PiOver2) {
            sinHigh = 1.f;
        }
        if (constexpr auto _3PiOver2 = (3.f / 2.f) * Pi;
            i.low() < _3PiOver2 && i.high() > _3PiOver2)
        {
            sinLow = -1.f;
        }

        return Interval{sinLow, sinHigh};
    }

    Interval cos(const Interval& i) {
        using constants::Pi;
        using constants::PiOver2;

        assert(i.low() >= 0.f);
        assert(i.high() <= 2.0001 * Pi);

        auto cosLow = std::cos(i.low());
        auto cosHigh = std::cos(i.high());

        if (cosLow > cosHigh) {
            std::swap(cosLow, cosHigh);
        }
        if (i.low() < Pi && i.high() > Pi) {
            cosLow = -1.f;
        }

        return Interval{cosLow, cosHigh};
    }
} // namespace idragnev::pbrt