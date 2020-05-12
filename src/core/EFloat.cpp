#include "EFloat.hpp"

#include <assert.h>
#include <algorithm>

namespace idragnev::pbrt {
    EFloat::EFloat(const float v, const float err) noexcept
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

    EFloat EFloat::operator-() const noexcept {
        EFloat result;

        result.v = -v;
        result.bounds = -bounds;

#ifndef NDEBUG
        result.vPrecise = -vPrecise;
#endif //!NDEBUG

        return result;
    }

    EFloat EFloat::operator+(const EFloat& rhs) const noexcept {
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
        assert(rhs.v != 0.f);

        EFloat result;
        result.v = v / rhs.v;

#ifndef NDEBUG
        result.vPrecise = vPrecise / rhs.vPrecise;
#endif //!NDEBUG

        if (rhs.lowerBound() < 0.f && rhs.upperBound() > 0.f) {
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

    EFloat EFloat::operator*(const EFloat& rhs) const noexcept {
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

    EFloat EFloat::operator-(const EFloat& rhs) const noexcept {
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

    EFloat sqrt(const EFloat& ef) {
        assert(ef.v >= 0.f);

        EFloat result;
        result.v = std::sqrt(ef.v);

#ifndef NDEBUG
        assert(ef.vPrecise >= 0.);
        result.vPrecise = std::sqrt(ef.vPrecise);
#endif

        assert(ef.lowerBound() >= 0.f);
        assert(ef.upperBound() >= 0.f);
        result.bounds = Interval{
            nextFloatDown(std::sqrt(ef.lowerBound())),
            nextFloatUp(std::sqrt(ef.upperBound())),
            Interval::NoOrderCheck{}
        };

        return result;
    }

    EFloat abs(const EFloat &ef) {
        if (ef.lowerBound() >= 0.f) {
            return ef;
        }
        else if (ef.upperBound() <= 0.f) {
            return -ef;
        }
        else {
            EFloat result;

            result.v = std::abs(ef.v);
            result.bounds = Interval{
                0.f,
                std::max(-ef.lowerBound(), ef.upperBound()),
                Interval::NoOrderCheck{}
            };

#ifndef NDEBUG
            result.vPrecise = std::abs(ef.vPrecise);
#endif

            return result;
        }
    }

    std::optional<QuadraticRoots> solveQuadratic(const EFloat& a, const EFloat& b, const EFloat& c) {
        const auto D = static_cast<double>(b) * static_cast<double>(b) -
                       4. * static_cast<double>(a) * static_cast<double>(c);
        if (D < 0.) {
            return std::nullopt;
        }

        const auto sqrtD = [D] {
            const auto d = std::sqrt(D);
            return EFloat(d, constants::MachineEpsilon * d);
        }();

        const auto q = (static_cast<float>(b) < 0.f)  ? EFloat{-0.5 * (b - sqrtD)}
                                                      : EFloat{-0.5 * (b + sqrtD)};
        const auto t0 = q / a;
        const auto t1 = c / q;

        return std::make_optional(
            static_cast<float>(t0) > static_cast<float>(t1) ? QuadraticRoots{ t1, t0 }
                                                            : QuadraticRoots{ t0, t1 }
        );
    }
} //namespace idragnev::pbrt