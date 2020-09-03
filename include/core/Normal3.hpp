#pragma once

#include <assert.h>

#include "core/core.hpp"

namespace idragnev::pbrt {
    template <typename T>
    class Normal3
    {
    public:
        Normal3() = default;
        Normal3(T x, T y, T z);
        explicit Normal3(const Vector3<T>& v);

#ifndef NDEBUG
        Normal3(const Normal3<T>& n) : Normal3{n.x, n.y, n.z} {}
        Normal3<T>& operator=(const Normal3<T>& n) {
            assert(!n.hasNaNs());
            if (&n != this) {
                x = n.x;
                y = n.y;
                z = n.z;
            }
            return *this;
        }
#endif // !NDEBUG

        bool hasNaNs() const noexcept;

        Float lengthSquared() const noexcept;
        Float length() const;

        template <typename U>
        Normal3<T>& operator*=(U f);
        template <typename U>
        Normal3<T>& operator/=(U f);

        Normal3<T>& operator+=(const Normal3<T>& n);
        Normal3<T>& operator-=(const Normal3<T>& n);

        T& operator[](std::size_t i);
        T operator[](std::size_t i) const;

        T x{};
        T y{};
        T z{};
    };

    template <typename T>
    Normal3<T> normalize(const Normal3<T>& n);

    template <typename T, typename U>
    Normal3<T> operator/(const Normal3<T>& n, U f);

    template <typename T>
    Normal3<T> operator-(const Normal3<T>& n);

    template <typename T>
    Normal3<T> operator+(const Normal3<T>& n1, const Normal3<T>& n2);

    template <typename T>
    Normal3<T> operator-(const Normal3<T>& n1, const Normal3<T>& n2);

    template <typename T, typename U>
    Normal3<T> operator*(U f, const Normal3<T>& n);

    template <typename T>
    bool operator==(const Normal3<T>& n1, const Normal3<T>& n2) noexcept;

    template <typename T>
    bool operator!=(const Normal3<T>& n1, const Normal3<T>& n2) noexcept;

    template <typename T>
    Normal3<T> abs(const Normal3<T>& v);

    template <typename T>
    T dot(const Normal3<T>& n, const Vector3<T>& v);

    template <typename T>
    T dot(const Vector3<T>& v, const Normal3<T>& n);

    template <typename T>
    T dot(const Normal3<T>& n1, const Normal3<T>& n2);

    template <typename T>
    T absDot(const Normal3<T>& n, const Vector3<T>& v);

    template <typename T>
    T absDot(const Vector3<T>& v, const Normal3<T>& n);

    template <typename T>
    T absDot(const Normal3<T>& n1, const Normal3<T>& n2);

    template <typename T, typename F>
    T faceforward(const T& u, const F& v);
} // namespace idragnev::pbrt

#include "core/Normal3Impl.hpp"