#pragma once

#include "core.hpp"

namespace idragnev::pbrt
{
    template <typename T>
    class Vector2
    {
    private:
        static_assert(std::is_arithmetic_v<T>, "Cannot instantiate Vector2 with non-arithmetic type");

    public:
        Vector2() = default;
        Vector2(T x, T y);
       
        explicit Vector2(const Point2<T>& p);
        explicit Vector2(const Point3<T>& p);

#ifndef NDEBUG
        //Added only for debug builds so asserts can be used
        Vector2(const Vector2& v) : Vector2{v.x, v.y} { }
        Vector2& operator=(const Vector2& v) {
            assert(!v.hasNaNs());
            x = v.x;
            y = v.y;
            return *this;
        }
#endif  // !NDEBUG

        bool hasNaNs() const noexcept;
        Float lengthSquared() const noexcept;
        Float length() const;

        template <typename U>
        Vector2<T>& operator*=(U f);
        template <typename U>
        Vector2<T>& operator/=(U f);

        Vector2<T>& operator+=(const Vector2<T>& v);
        Vector2<T>& operator-=(const Vector2<T>& v);

        T& operator[](std::size_t i);
        T operator[](std::size_t i) const;

    public:
        T x{};
        T y{};
    };

    template <typename T>
    Float Dot(const Vector2<T>& u, const Vector2<T>& v);

    template <typename T>
    Float AbsDot(const Vector2<T>& u, const Vector2<T>& v);
    
    template <typename T>
    Vector2<T> Normalize(const Vector2<T>& v) noexcept;
    
    template <typename T>
    Vector2<T> Abs(const Vector2<T>& v);

    template <typename T>
    Vector2<T> operator+(const Vector2<T>& u, const Vector2<T>& v);

    template <typename T>
    Vector2<T> operator-(const Vector2<T>& u, const Vector2<T>& v);
    
    template <typename T, typename U>
    Vector2<T> operator*(const U f, const Vector2<T>& u);

    template <typename T, typename U>
    Vector2<T> operator/(const Vector2<T>& u, const U f);

    template <typename T>
    Vector2<T> operator-(const Vector2<T>& v);

    template <typename T>
    bool operator==(const Vector2<T>& u, const Vector2<T>& v) noexcept;

    template <typename T>
    bool operator!=(const Vector2<T>& u, const Vector2<T>& v) noexcept;
} //namespace idragnev::pbrt

#include "Vector2Impl.hpp"