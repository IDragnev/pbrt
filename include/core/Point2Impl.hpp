#pragma once

#include <algorithm>

namespace idragnev::pbrt {
    template <typename T>
    inline Point2<T>::Point2(T x, T y) : x{x}
                                       , y{y} {
        assert(!hasNaNs());
    }

    template <typename T>
    inline bool Point2<T>::hasNaNs() const noexcept {
        return isNaN(x) || isNaN(y);
    }

    template <typename T>
    template <typename U>
    inline Point2<T>::operator Vector2<U>() const {
        static_assert(std::is_convertible_v<T, U>,
                      "Cannot convert the underlying type");
        return Vector2<U>{x, y};
    }

    template <typename T>
    template <typename U>
    inline Point2<T>::Point2(const Point2<U>& p)
        : Point2{static_cast<T>(p.x), static_cast<T>(p.y)} {
        static_assert(std::is_convertible_v<U, T>,
                      "Cannot convert the underlying type");
    }

    template <typename T>
    template <typename U>
    inline Point2<T>::Point2(const Vector2<U>& v)
        : Point2{static_cast<T>(v.x), static_cast<T>(v.y)} {
        static_assert(std::is_convertible_v<U, T>,
                      "Cannot convert the underlying type");
    }

    template <typename T>
    inline Point2<T>::Point2(const Point3<T>& p) : Point2{p.x, p.y} {}

    template <typename T>
    inline T Point2<T>::operator[](std::size_t i) const {
        assert(i < 2);
        return i == 0 ? x : y;
    }

    template <typename T>
    inline T& Point2<T>::operator[](std::size_t i) {
        assert(i < 2);
        return i == 0 ? x : y;
    }

    template <typename T>
    Point2<T>& Point2<T>::operator-=(const Point2& p) {
        assert(!p.hasNaNs());
        x -= p.x;
        y -= p.y;
        return *this;
    }

    template <typename T>
    Point2<T>& Point2<T>::operator+=(const Point2& p) {
        assert(!p.hasNaNs());
        x += p.x;
        y += p.y;
        return *this;
    }

    template <typename T>
    Point2<T>& Point2<T>::operator-=(const Vector2<T>& v) {
        assert(!v.hasNaNs());
        x -= v.x;
        y -= v.y;
        return *this;
    }

    template <typename T>
    Point2<T>& Point2<T>::operator+=(const Vector2<T>& v) {
        assert(!v.hasNaNs());
        x += v.x;
        y += v.y;
        return *this;
    }

    template <typename T>
    template <typename U>
    Point2<T>& Point2<T>::operator*=(U f) {
        static_assert(std::is_arithmetic_v<U>,
                      "Cannot scale a point with non-arithmetic type");
        assert(!isNaN(f));

        x *= f;
        y *= f;
        return *this;
    }

    template <typename T>
    template <typename U>
    Point2<T>& Point2<T>::operator/=(U f) {
        static_assert(std::is_arithmetic_v<U>,
                      "Cannot divide a point with non-arithmetic type");
        assert(f != 0);

        Float inv = Float{1} / f;
        return *this *= inv;
    }

    template <typename T>
    inline bool operator==(const Point2<T>& p1, const Point2<T>& p2) noexcept {
        return p1.x == p2.x && p1.y == p2.y;
    }

    template <typename T>
    inline bool operator!=(const Point2<T>& p1, const Point2<T>& p2) noexcept {
        return !(p1 == p2);
    }

    template <typename T, typename U>
    inline Point2<T> operator*(const U f, const Point2<T>& p) {
        auto temp = p;
        temp *= f;
        return temp;
    }

    template <typename T, typename U>
    inline Point2<T> operator/(const Point2<T>& p, const U f) {
        assert(f != 0);
        auto temp = p;
        temp /= f;
        return temp;
    }

    template <typename T>
    inline Point2<T> operator+(const Point2<T>& p, const Vector2<T>& v) {
        auto temp = p;
        temp += v;
        return temp;
    }

    template <typename T>
    inline Point2<T> operator+(const Point2<T>& p1, const Point2<T>& p2) {
        return p1 + Vector2<T>{p2};
    }

    template <typename T>
    Vector2<T> operator-(const Point2<T>& p1, const Point2<T>& p2) {
        auto temp = p1;
        temp -= p2;
        return Vector2<T>{temp};
    }

    template <typename T>
    Point2<T> operator-(const Point2<T>& p, const Vector2<T>& v) {
        auto temp = p;
        temp -= v;
        return temp;
    }

    template <typename T>
    inline Point2<T> operator-(const Point2<T>& p) {
        return {-p.x, -p.y};
    }

    template <typename T>
    inline Point2<T> abs(const Point2<T>& p) {
        return Point2<T>(std::abs(p.x), std::abs(p.y));
    }

    template <typename T>
    inline Float distance(const Point2<T>& p1, const Point2<T>& p2) {
        return (p1 - p2).length();
    }

    template <typename T>
    inline Float distanceSquared(const Point2<T>& p1, const Point2<T>& p2) {
        return (p1 - p2).lengthSquared();
    }

    template <typename T>
    inline Point2<T> floor(const Point2<T>& p) {
        return {std::floor(p.x), std::floor(p.y)};
    }

    template <typename T>
    inline Point2<T> ceil(const Point2<T>& p) {
        return {std::ceil(p.x), std::ceil(p.y)};
    }

    template <typename T>
    inline Point2<T>
    lerp(Float t, const Point2<T>& p0, const Point2<T>& p1) noexcept {
        return (1 - t) * p0 + t * p1;
    }

    template <typename T>
    inline Point2<T> min(const Point2<T>& pa, const Point2<T>& pb) {
        return {std::min(pa.x, pb.x), std::min(pa.y, pb.y)};
    }

    template <typename T>
    inline Point2<T> max(const Point2<T>& pa, const Point2<T>& pb) {
        return {std::max(pa.x, pb.x), std::max(pa.y, pb.y)};
    }

    template <typename T>
    inline Point2<T> permute(const Point2<T>& p, std::size_t x, std::size_t y) {
        return {p[x], p[y]};
    }
} // namespace idragnev::pbrt