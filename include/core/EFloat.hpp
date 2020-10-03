#pragma once

#include "core.hpp"
#include "Interval.hpp"

#include <optional>

namespace idragnev::pbrt {
    class EFloat
    {
    private:
        friend EFloat abs(const EFloat& fe);
        friend EFloat sqrt(const EFloat& fe);

        using ErrBounds = Interval<float>;

    public:
        EFloat() = default;
        EFloat(const float v, const float err = 0.f) noexcept;

#ifndef NDEBUG
        EFloat(const float v,
               const long double precise,
               const float err) noexcept
            : EFloat(v, err) {
            vPrecise = precise;
        }
#endif //! NDEBUG

        explicit operator float() const noexcept { return v; }
        explicit operator double() const noexcept { return v; }

        float upperBound() const noexcept { return bounds.high(); }
        float lowerBound() const noexcept { return bounds.low(); }

        float absoluteError() const;

#ifndef NDEBUG
        long double preciseValue() const noexcept { return vPrecise; }

        float relativeError() const noexcept {
            return std::abs((vPrecise - v) / vPrecise);
        }
#endif //! NDEBUG

        EFloat operator-() const noexcept;
        EFloat operator+(const EFloat& rhs) const noexcept;
        EFloat operator-(const EFloat& rhs) const noexcept;
        EFloat operator*(const EFloat& rhs) const noexcept;
        EFloat operator/(const EFloat& rhs) const;

        bool operator==(const EFloat& rhs) const noexcept { return v == rhs.v; }

    private:
        float v = 0.f;
        ErrBounds bounds = {0.f, 0.f, NoOrderCheck{}};

#ifndef NDEBUG
        long double vPrecise = 0.;
#endif // NDEBUG
    };

    inline EFloat operator*(const float lhs, const EFloat& rhs) {
        return EFloat{lhs} * rhs;
    }
    inline EFloat operator/(const float lhs, const EFloat& rhs) {
        return EFloat{lhs} / rhs;
    }
    inline EFloat operator+(const float lhs, const EFloat& rhs) {
        return EFloat{lhs} + rhs;
    }
    inline EFloat operator-(const float lhs, const EFloat& rhs) {
        return EFloat{lhs} - rhs;
    }

    EFloat sqrt(const EFloat& fe);
    EFloat abs(const EFloat& fe);

    struct QuadraticRoots
    {
        EFloat t0;
        EFloat t1;
    };

    std::optional<QuadraticRoots>
    solveQuadratic(const EFloat& a, const EFloat& b, const EFloat& c);
} // namespace idragnev::pbrt