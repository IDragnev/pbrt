#pragma once

namespace idragnev::pbrt {
    template <typename T>
    inline Vector2<T>::Vector2(T x, T y) 
        : x{x}
        , y{y} 
    { 
        assert(!hasNaNs()); 
    }

    template <typename T>
    inline bool Vector2<T>::hasNaNs() const noexcept { 
        return isNaN(x) || isNaN(y); 
    }

    template <typename T>
    inline Vector2<T>::Vector2(const Point2<T>& p)
        : Vector2{p.x, p.y} 
    {
    }

    template <typename T>
    inline Vector2<T>::Vector2(const Point3<T>& p)
        : Vector2{p.x, p.y} 
    {
    }

    template <typename T>
    inline Float Vector2<T>::lengthSquared() const noexcept { 
        return x * x + y * y; 
    }

    template <typename T>
    inline Float Vector2<T>::length() const { 
        return std::sqrt(lengthSquared()); 
    }

    template <typename T>
    inline T Vector2<T>::operator[](std::size_t i) const {
        assert(i <= 1);
        return i == 0 ? x : y;
    }

    template <typename T>
    inline T& Vector2<T>::operator[](std::size_t i) {
        assert(i <= 1);
        return i == 0 ? x : y;
    }

    template <typename T>
    Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& v) {
        assert(!v.hasNaNs());
        x -= v.x;
        y -= v.y;
        return *this;
    }

    template <typename T>
    Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& v) {
        assert(!v.hasNaNs());
        x += v.x;
        y += v.y;
        return *this;
    }

    template <typename T>
    template <typename U>
    Vector2<T>& Vector2<T>::operator*=(U f) {
        static_assert(std::is_arithmetic_v<U>, "Cannot scale a vector with non-arithmetic type");
        assert(!isNaN(f));

        x *= f;
        y *= f;
        return *this;
    }

    template <typename T>
    template <typename U>
    Vector2<T>& Vector2<T>::operator/=(U f) {
        static_assert(std::is_arithmetic_v<U>, "Cannot divide a vector with non-arithmetic type");
        assert(f != 0);

        Float inv = Float{1} / f;
        return *this *= inv;
    }

    template <typename T>
    Vector2<T> operator+(const Vector2<T>&  u, const Vector2<T>& v) {
        auto temp = u;
        temp += v;
        return temp;
    }
    
    template <typename T>
    Vector2<T> operator-(const Vector2<T>& u, const Vector2<T>& v) {
        auto temp = u;
        temp -= v;
        return temp;
    }
    
    template <typename T>
    inline bool operator==(const Vector2<T>& u, const Vector2<T>& v) noexcept { 
        return u.x == v.x && u.y == v.y; 
    }

    template <typename T>
    inline bool operator!=(const Vector2<T>& u, const Vector2<T>& v) noexcept {
        return !(u == v);
    }

    template <typename T, typename U>
    Vector2<T> operator*(const U f, const Vector2<T>& v) {
        auto temp = v;
        temp *= f;
        return temp;
    }

    template <typename T, typename U>
    Vector2<T> operator/(const Vector2<T>& v, const U f) {
        assert(f != 0);
        auto temp = v;
        temp /= f;
        return temp;
    }

    template <typename T>
    inline Vector2<T> operator-(const Vector2<T>& v) { 
        return Vector2<T>(-v.x, -v.y);
    }

    template <typename T>
    inline Float dot(const Vector2<T>& u, const Vector2<T>& v) {
        assert(!u.hasNaNs() && !v.hasNaNs());
        return u.x * v.x + u.y * v.y;
    }

    template <typename T>
    inline Float absDot(const Vector2<T>& u, const Vector2<T>& v) {
        assert(!u.hasNaNs() && !v.hasNaNs());
        return std::abs(Dot(u, v));
    }

    template <typename T>
    inline Vector2<T> normalize(const Vector2<T>& v) noexcept {
        return v / v.length();
    }

    template <typename T>
    inline Vector2<T> Abs(const Vector2<T>& v) {
        return Vector2<T>(std::abs(v.x), std::abs(v.y));
    }

} //namespace idragnev::pbrt 