#include "pbrt/core/Interval.hpp"

#include <algorithm>
#include <assert.h>

namespace idragnev::pbrt {
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
} // namespace idragnev::pbrt