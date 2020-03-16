#pragma once

#include <assert.h>

#include "core.hpp"

namespace idragnev::pbrt {
    template <typename T>
    struct Basis3
    {
        static_assert(std::is_arithmetic_v<T>, "Cannot instantiate Basis3 with non-arithmetic type");

        Vector3<T> u;
        Vector3<T> v;
        Vector3<T> w;
    };

    template <typename T>
    class Vector3
    {
    private:
        static_assert(std::is_arithmetic_v<T>, "Cannot instantiate Vector3 with non-arithmetic type");

    public:
        Vector3() = default;
        Vector3(T x, T y, T z);

        explicit Vector3(const Point3<T>& p);
        explicit Vector3(const Normal3<T>& n);

#ifndef NDEBUG
        Vector3(const Vector3& v) : Vector3{v.x, v.y, v.z} { }
        Vector3& operator=(const Vector3& v) {
            assert(!v.hasNaNs());
            if (&v != this) {
                x = v.x;
                y = v.y;
                z = v.z;
            }
            return *this;
        }
#endif  // !NDEBUG

        bool hasNaNs() const noexcept;
        Float lengthSquared() const noexcept;
        Float length() const;

        template <typename U>
        Vector3<T>& operator*=(U f);
        template <typename U>
        Vector3<T>& operator/=(U f);

        Vector3<T>& operator+=(const Vector3<T>& v);
        Vector3<T>& operator-=(const Vector3<T>& v);

        T& operator[](std::size_t i);
        T operator[](std::size_t i) const;

    public:
        T x{};
        T y{};
        T z{};
    };

    template <typename T>
    Float dot(const Vector3<T>& u, const Vector3<T>& v);

    template <typename T>
    Float absDot(const Vector3<T>& u, const Vector3<T>& v);

    template <typename T>
    Vector3<T> normalize(const Vector3<T>& v) noexcept;

    template <typename T>
    Vector3<T> abs(const Vector3<T>& v);

    template <typename T>
    Vector3<T> cross(const Vector3<T>& v1, const Vector3<T>& v2);

    template <typename T>
    Vector3<T> cross(const Vector3<T>& v1, const Normal3<T>& v2);

    template <typename T>
    Vector3<T> cross(const Normal3<T>& v1, const Vector3<T>& v2);

    template <typename T>
    T minComponent(const Vector3<T>& v) noexcept;

    template <typename T>
    T maxComponent(const Vector3<T>& v) noexcept;

    template <typename T>
    std::size_t maxDimension(const Vector3<T>& v) noexcept;

    template <typename T>
    Vector3<T> min(const Vector3<T>& p1, const Vector3<T>& p2);

    template <typename T>
    Vector3<T> max(const Vector3<T>& p1, const Vector3<T>& p2);

    template <typename T>
    Vector3<T> permute(const Vector3<T>& v, std::size_t x, std::size_t y, std::size_t z);

    template <typename T>
    Basis3<T> coordinateSystem(const Vector3<T>& v);

    template <typename T>
    Vector3<T> operator+(const Vector3<T>& u, const Vector3<T>& v);

    template <typename T>
    Vector3<T> operator-(const Vector3<T>& u, const Vector3<T>& v);

    template <typename T, typename U>
    Vector3<T> operator*(const U f, const Vector3<T>& u);

    template <typename T, typename U>
    Vector3<T> operator/(const Vector3<T>& u, const U f);

    template <typename T>
    Vector3<T> operator-(const Vector3<T>& v);

    template <typename T>
    bool operator==(const Vector3<T>& u, const Vector3<T>& v) noexcept;

    template <typename T>
    bool operator!=(const Vector3<T>& u, const Vector3<T>& v) noexcept;
} //namespace idragnev::pbrt

#include "Vector3Impl.hpp"