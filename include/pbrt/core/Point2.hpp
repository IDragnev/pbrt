#pragma once

#include <type_traits>
#include <assert.h>

#include "core.hpp"

namespace idragnev::pbrt {
    template <typename T>
    class Point2
    {
    private:
        static_assert(std::is_arithmetic_v<T>,
                      "Cannot instantiate Point2 with non-arithmetic type");

    public:
        using UnderlyingType = T;

        Point2() = default;
        Point2(T xx, T yy);

        explicit Point2(const Point3<T>& p);

        template <typename U>
        explicit Point2(const Point2<U>& p);

        template <typename U>
        explicit Point2(const Vector2<U>& v);

        template <typename U>
        explicit operator Vector2<U>() const;

#ifndef NDEBUG
        // Added only for debug builds so asserts can be used
        Point2(const Point2& p) : Point2{p.x, p.y} {}
        Point2& operator=(const Point2& p) {
            assert(!p.hasNaNs());
            if (&p != this) {
                x = p.x;
                y = p.y;
            }
            return *this;
        }
#endif // !NDEBUG

        bool hasNaNs() const noexcept;

        template <typename U>
        Point2& operator*=(U f);
        template <typename U>
        Point2& operator/=(U f);

        // for easier use
        Point2& operator+=(const Point2& p);
        Point2& operator-=(const Point2& p);

        Point2& operator+=(const Vector2<T>& v);
        Point2& operator-=(const Vector2<T>& v);

        T& operator[](std::size_t i);
        T operator[](std::size_t i) const;

    public:
        T x{};
        T y{};
    };

    template <typename T>
    Point2<T> operator+(const Point2<T>& p, const Vector2<T>& v);

    // for easier use
    template <typename T>
    Point2<T> operator+(const Point2<T>& p1, const Point2<T>& p2);

    template <typename T>
    Vector2<T> operator-(const Point2<T>& p1, const Point2<T>& p2);

    template <typename T>
    Point2<T> operator-(const Point2<T>& p, const Vector2<T>& v);

    template <typename T>
    Point2<T> operator-(const Point2<T>& p);

    template <typename T>
    bool operator==(const Point2<T>& p1, const Point2<T>& p2) noexcept;

    template <typename T>
    bool operator!=(const Point2<T>& p1, const Point2<T>& p2) noexcept;

    template <typename T, typename U>
    Point2<T> operator*(const U f, const Point2<T>& p);

    template <typename T, typename U>
    Point2<T> operator/(const Point2<T>& p, const U f);

    template <typename T>
    Point2<T> abs(const Point2<T>& p);

    template <typename T>
    Float distance(const Point2<T>& p1, const Point2<T>& p2);

    template <typename T>
    Float distanceSquared(const Point2<T>& p1, const Point2<T>& p2);

    template <typename T>
    Point2<T> floor(const Point2<T>& p);

    template <typename T>
    Point2<T> ceil(const Point2<T>& p);

    template <typename T>
    Point2<T> lerp(Float t, const Point2<T>& p0, const Point2<T>& p1) noexcept;

    template <typename T>
    Point2<T> min(const Point2<T>& p1, const Point2<T>& p2);

    template <typename T>
    Point2<T> max(const Point2<T>& p1, const Point2<T>& p2);

    template <typename T>
    Point2<T> permute(const Point2<T>& p, std::size_t x, std::size_t y);
} // namespace idragnev::pbrt

#include "Point2Impl.hpp"