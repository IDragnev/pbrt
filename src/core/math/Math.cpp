#include "pbrt/core/math/Math.hpp"
#include "pbrt/core/math/Interval.hpp"

#include <algorithm>
#include <assert.h>

namespace idragnev::pbrt::math {
    Intervalf sin(const Intervalf& i) {
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

        return Intervalf{sinLow, sinHigh};
    }

    Intervalf cos(const Intervalf& i) {
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

        return Intervalf{cosLow, cosHigh};
    }

    void extendedGCD(std::uint64_t a,
                     std::uint64_t b,
                     std::int64_t& x,
                     std::int64_t& y) {
        if (b == 0) {
            x = 1;
            y = 0;

            return;
        }

        const std::int64_t d = a / b;
        std::int64_t xp = 0;
        std::int64_t yp = 0;

        extendedGCD(b, a % b, xp, yp);
        x = yp;
        y = xp - (d * yp);
    }

    std::uint64_t multInverseCoprimes(const std::int64_t a,
                                      const std::int64_t n) {
        std::int64_t x = 0;
        std::int64_t y = 0;

        // ax + ny = gcd(a, n) 
        extendedGCD(a, n, x, y);

        return mod(x, n);
    }
} // namespace idragnev::pbrt::math