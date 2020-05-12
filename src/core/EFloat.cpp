#include "EFloat.hpp"

#include <assert.h>
#include <algorithm>

namespace idragnev::pbrt {
    EFloat::EFloat(const float v, const float err) 
        : v(v)
        , bounds{
            [err, v] {
                return err == 0.f ? Interval{v}
                                  : Interval{
                                      nextFloatDown(v - err), 
                                      nextFloatUp(v + err),
                                      Interval::NoOrderCheck{}
                                    };
            }()
        }
#ifndef NDEBUG
        , vPrecise(v)
#endif //!NDEBUG
    {
    }

    float EFloat::absoluteError() const { 
        return nextFloatUp(
            std::max(
                std::abs(bounds.high() - v), 
                std::abs(v - bounds.low())
            )
        ); 
    }

    EFloat EFloat::operator-() const {
        EFloat result;

        result.v = -v;
        result.bounds = -bounds;

#ifndef NDEBUG
        result.vPrecise = -vPrecise;
#endif //!NDEBUG

        return result;
    }

    EFloat EFloat::operator+(const EFloat& rhs) const {
        EFloat result;

        result.v = v + rhs.v;

        const auto b = bounds + rhs.bounds;
        result.bounds = Interval{
            nextFloatDown(b.low()),
            nextFloatUp(b.high()),
            Interval::NoOrderCheck{}
        };

#ifndef NDEBUG
        result.vPrecise = vPrecise + rhs.vPrecise;
#endif //!NDEBUG

        return result;
    }

    EFloat EFloat::operator/(const EFloat& rhs) const {
        EFloat result;
        
        result.v = v / rhs.v;

#ifndef NDEBUG
        result.vPrecise = vPrecise / rhs.vPrecise;
#endif //!NDEBUG

        if (rhs.lowerBound() < 0.f && rhs.upperBound() > 0.f) {
            //the interval we're dividing by straddles zero
            //so just return an interval of everything
            result.bounds = Interval{
                -constants::Infinity,
                 constants::Infinity,
                Interval::NoOrderCheck{}
            };
        } else {
            const auto [min, max] = std::minmax({
                lowerBound() / rhs.lowerBound(),
                lowerBound() / rhs.upperBound(),
                upperBound() / rhs.lowerBound(),
                upperBound() / rhs.upperBound()
            });
            result.bounds = Interval{
                nextFloatDown(min),
                nextFloatUp(max),
                Interval::NoOrderCheck{}
            };
        }

        return result;
    }

    EFloat EFloat::operator*(const EFloat& rhs) const {
        EFloat result;

        result.v = v * rhs.v;

        const auto b = bounds * rhs.bounds;
        result.bounds = Interval{
            nextFloatDown(b.low()),
            nextFloatUp(b.high()),
            Interval::NoOrderCheck{}
        };

#ifndef NDEBUG
        result.vPrecise = vPrecise * rhs.vPrecise;
#endif

        return result;
    }

    EFloat EFloat::operator-(const EFloat& rhs) const {
        EFloat result;

        result.v = v - rhs.v;

        const auto b = bounds - rhs.bounds;
        result.bounds = Interval{
            nextFloatDown(b.low()),
            nextFloatUp(b.high()),
            Interval::NoOrderCheck{}
        };

#ifndef NDEBUG
        result.vPrecise = vPrecise - rhs.vPrecise;
#endif

        return result;
    }

    EFloat sqrt(const EFloat& fe) {
        EFloat result;

        result.v = std::sqrt(fe.v);

#ifndef NDEBUG
        result.vPrecise = std::sqrt(fe.vPrecise);
#endif

        assert(fe.lowerBound() >= 0.f);
        assert(fe.upperBound() >= 0.f);
        result.bounds = Interval{
            nextFloatDown(std::sqrt(fe.lowerBound())),
            nextFloatUp(std::sqrt(fe.upperBound())),
            Interval::NoOrderCheck{}
        };

        return result;
    }
} //namespace idragnev::pbrt