#pragma once

#include <algorithm>

namespace idragnev::pbrt {
    template <typename T>
    inline Point3<T>::Point3(T x, T y, T z)
        : x{x}
        , y{y}
        , z{z}
    {
        assert(!hasNaNs());
    }

    template <typename T>
    inline bool Point3<T>::hasNaNs() const noexcept {
        return isNaN(x) || isNaN(y) || isNaN(z);
    }

    template <typename T>
    template <typename U>
    inline Point3<T>::operator Vector3<U>() const {
        static_assert(std::is_convertible_v<T, U>, "Cannot convert the underlying type");
        return Vector3<U>{x, y, z};
    }

    template <typename T>
    template <typename U>
    inline Point3<T>::Point3(const Point3<U>& p)
        : Point3{static_cast<T>(p.x), static_cast<T>(p.y), static_cast<T>(p.z)}
    {
        static_assert(std::is_convertible_v<U, T>, "Cannot convert the underlying type");
    }

    template <typename T>
    template <typename U>
    inline Point3<T>::Point3(const Vector3<U>& v)
        : Point3{static_cast<T>(v.x), static_cast<T>(v.y), static_cast<T>(v.z)}
    {
        static_assert(std::is_convertible_v<U, T>, "Cannot convert the underlying type");
    }

    template <typename T>
    T Point3<T>::operator[](std::size_t i) const {
        assert(i < 3);
        switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: assert(false);
        }
    }

    template <typename T>
    T& Point3<T>::operator[](std::size_t i) {
        assert(i < 3);
        switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: assert(false);
        }
    }

    template <typename T>
    Point3<T>& Point3<T>::operator-=(const Point3& p) {
        assert(!p.hasNaNs());
        x -= p.x;
        y -= p.y;
        z -= p.z;
        return *this;
    }

    template <typename T>
    Point3<T>& Point3<T>::operator+=(const Point3& p) {
        assert(!p.hasNaNs());
        x += p.x;
        y += p.y;
        z += p.z;
        return *this;
    }

    template <typename T>
    Point3<T>& Point3<T>::operator-=(const Vector3<T>& v) {
        assert(!v.hasNaNs());
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    template <typename T>
    Point3<T>& Point3<T>::operator+=(const Vector3<T>& v) {
        assert(!v.hasNaNs());
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    template <typename T>
    template <typename U>
    Point3<T>& Point3<T>::operator*=(U f) {
        static_assert(std::is_arithmetic_v<U>, "Cannot scale a point with non-arithmetic type");
        assert(!isNaN(f));

        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    template <typename T>
    template <typename U>
    Point3<T>& Point3<T>::operator/=(U f) {
        static_assert(std::is_arithmetic_v<U>, "Cannot divide a point with non-arithmetic type");
        assert(f != 0);

        Float inv = Float{1} / f;
        return *this *= inv;
    }

    template <typename T>
    inline bool operator==(const Point3<T>& p1, const Point3<T>& p2) noexcept {
        return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
    }

    template <typename T>
    inline bool operator!=(const Point3<T>& p1, const Point3<T>& p2) noexcept {
        return !(p1 == p2);
    }

    template <typename T, typename U>
    inline Point3<T> operator*(const U f, const Point3<T>& p) {
        auto temp = p;
        temp *= f;
        return temp;
    }

    template <typename T, typename U>
    inline Point3<T> operator/(const Point3<T>& p, const U f) {
        assert(f != 0);
        auto temp = p;
        temp /= f;
        return temp;
    }

    template <typename T>
    inline Point3<T> operator+(const Point3<T>& p, const Vector3<T>& v) {
        auto temp = p;
        temp += v;
        return temp;
    }

    template <typename T>
    inline Point3<T> operator+(const Point3<T>& p1, const Point3<T>& p2) {
        return p1 + Vector3<T>{p2};
    }

    template <typename T>
    Vector3<T> operator-(const Point3<T>& p1, const Point3<T>& p2) {
        auto temp = p1;
        temp -= p2;
        return Vector3<T>{temp};
    }

    template <typename T>
    Point3<T> operator-(const Point3<T>& p, const Vector3<T>& v) {
        auto temp = p;
        temp -= v;
        return temp;
    }

    template <typename T>
    inline Point3<T> operator-(const Point3<T>& p) {
        return {-p.x, -p.y, -p.z};
    }

    template <typename T>
    inline Point3<T> abs(const Point3<T>& p) {
        return Point3<T>(std::abs(p.x), std::abs(p.y), std::abs(p.z));
    }

    template <typename T>
    inline Float distance(const Point3<T>& p1, const Point3<T>& p2) {
        return (p1 - p2).length();
    }

    template <typename T>
    inline Float distanceSquared(const Point3<T>& p1, const Point3<T>& p2) {
        return (p1 - p2).lengthSquared();
    }

    template <typename T>
    inline Point3<T> floor(const Point3<T>& p) {
        return {std::floor(p.x), std::floor(p.y), std::floor(p.z)};
    }

    template <typename T>
    inline Point3<T> ceil(const Point3<T>& p) {
        return {std::ceil(p.x), std::ceil(p.y), std::ceil(p.z)};
    }

    template <typename T>
    inline Point3<T> lerp(Float t, const Point3<T>& p0, const Point3<T>& p1) noexcept {
        return (1 - t)*p0 + t*p1;
    }

    template <typename T>
    inline Point3<T> min(const Point3<T>& pa, const Point3<T>& pb) {
        return {std::min(pa.x, pb.x), std::min(pa.y, pb.y), std::min(pa.z, pb.z)};
    }

    template <typename T>
    inline Point3<T> max(const Point3<T>& pa, const Point3<T>& pb) {
        return {std::max(pa.x, pb.x), std::max(pa.y, pb.y), std::max(pa.z, pb.z) };
    }

    template <typename T>
    inline Point3<T> permute(const Point3<T>& p, std::size_t x, std::size_t y, std::size_t z) {
        return {p[x], p[y], p[z]};
    }
} //namespace idragnev::pbrt 