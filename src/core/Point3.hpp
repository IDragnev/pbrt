#pragma once

#include <type_traits>
#include <assert.h>

#include "core.hpp"

namespace idragnev::pbrt {
    template <typename T>
    class Vector3;

    template <typename T>
    class Point3 
    {
    private:
        static_assert(std::is_arithmetic_v<T>, "Cannot instantiate Point3 with non-arithmetic type");

    public:
        Point3() = default;
        Point3(T x, T y, T z);

        template <typename U>
        explicit Point3(const Point3<U>& p);

        template <typename U>
        explicit Point3(const Vector3<U>& v);

        template <typename U>
        explicit operator Vector3<U>() const;

#ifndef NDEBUG
        //Added only for debug builds so asserts can be used
        Point3(const Point3& p) : Point3{p.x, p.y, p.z} { }
        Point3& operator=(const Point3& p) {
            assert(!p.hasNaNs());
            if (&p != this) {
                x = p.x;
                y = p.y;
                z = p.z;
            }
            return *this;
        }
#endif  // !NDEBUG

        bool hasNaNs() const noexcept;

        template <typename U>
        Point3& operator*=(U f);
        template <typename U>
        Point3& operator/=(U f);

        //for easier use
        Point3& operator+=(const Point3& p);
        Point3& operator-=(const Point3& p);

        Point3& operator+=(const Vector3<T>& v);
        Point3& operator-=(const Vector3<T>& v);

        T& operator[](std::size_t i);
        T operator[](std::size_t i) const;

    public:
        T x{};
        T y{};
        T z{};
    };

    template <typename T>
    Point3<T> operator+(const Point3<T>& p, const Vector3<T>& v);

    //for easier use
    template <typename T>
    Point3<T> operator+(const Point3<T>& p1, const Point3<T>& p2);

    template <typename T>
    Vector3<T> operator-(const Point3<T>& p1, const Point3<T>& p2);

    template <typename T>
    Point3<T> operator-(const Point3<T>& p, const Vector3<T>& v);

    template <typename T>
    Point3<T> operator-(const Point3<T>& p);

    template <typename T>
    bool operator==(const Point3<T>& p1, const Point3<T>& p2) noexcept;

    template <typename T>
    bool operator!=(const Point3<T>& p1, const Point3<T>& p2) noexcept;

    template <typename T, typename U>
    Point3<T> operator*(const U f, const Point3<T>& p);

    template <typename T, typename U>
    Point3<T> operator/(const Point3<T>& p, const U f);

    template <typename T>
    Point3<T> abs(const Point3<T>& p);

    template <typename T>
    Float distance(const Point3<T>& p1, const Point3<T>& p2);

    template <typename T>
    Float distanceSquared(const Point3<T>& p1, const Point3<T>& p2);

    template <typename T>
    Point3<T> floor(const Point3<T>& p);

    template <typename T>
    Point3<T> ceil(const Point3<T>& p);

    template <typename T>
    Point3<T> lerp(Float t, const Point3<T>& p0, const Point3<T>& p1) noexcept;

    template <typename T>
    Point3<T> min(const Point3<T>& p1, const Point3<T>& p2);

    template <typename T>
    Point3<T> max(const Point3<T>& p1, const Point3<T>& p2);

    template <typename T>
    Point3<T> permute(const Point3<T>& p, std::size_t x, std::size_t y, std::size_t z);
}

#include "Point3Impl.hpp"