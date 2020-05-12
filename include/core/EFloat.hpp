#pragma once

#include "core.hpp"
#include "Interval.hpp"

#include <optional>

namespace idragnev::pbrt {
    struct QuadraticRoots;

    class EFloat
    {
    private:
        friend EFloat abs(const EFloat& fe);
        friend EFloat sqrt(const EFloat& fe);
        friend std::optional<QuadraticRoots> quadratic(const EFloat& a, const EFloat& b, const EFloat& c);

    public:
        EFloat() = default;
        EFloat(const float v, const float err = 0.f);

    #ifndef NDEBUG
        EFloat(const float v, const long double precise, const float err) 
            : EFloat(v, err)
        {
            vPrecise = precise;
        }
    #endif //!NDEBUG

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
    #endif //!NDEBUG

        EFloat operator-() const;
        EFloat operator+(const EFloat& rhs) const;
        EFloat operator-(const EFloat& rhs) const;
        EFloat operator*(const EFloat& rhs) const;
        EFloat operator/(const EFloat& rhs) const;

        bool operator==(const EFloat& rhs) const noexcept { return v == rhs.v; }

    private:
        float v = 0.f;
        Interval bounds = {0.f, 0.f, Interval::NoOrderCheck{}};

    #ifndef NDEBUG
        long double vPrecise = 0.;
    #endif //NDEBUG

    };

    struct QuadraticRoots 
    {
        EFloat t0;
        EFloat t1;
    };

    EFloat operator*(const float lhs, const EFloat& rhs) { return EFloat{lhs} * rhs; }
    EFloat operator/(const float lhs, const EFloat& rhs) { return EFloat{lhs} / rhs; }
    EFloat operator+(const float lhs, const EFloat& rhs) { return EFloat{lhs} + rhs; }
    EFloat operator-(const float lhs, const EFloat& rhs) { return EFloat{lhs} - rhs; }

    EFloat sqrt(const EFloat& fe);
    EFloat abs(const EFloat& fe);

    std::optional<QuadraticRoots> quadratic(const EFloat& a, const EFloat& b, const EFloat& c);
} //namespace idragnev::pbrt