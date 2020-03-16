#pragma once

namespace idragnev::pbrt {
    template <typename T>
    inline Normal3<T>::Normal3(const Vector3<T>& v) 
        : Normal3{v.x, v.y, v.z}
    {
    }

    template <typename T>
    inline Normal3<T>::Normal3(T x, T y, T z)
        : x{x}
        , y{y}
        , z{z}
    {
        assert(!hasNaNs());
    }

    template <typename T>
    inline bool Normal3<T>::hasNaNs() const noexcept {
        return isNaN(x) || isNaN(y) || isNaN(z);
    }

    template <typename T>
    inline Float Normal3<T>::lengthSquared() const noexcept {
        return x*x + y*y + z*z;
    }

    template <typename T>
    inline Float Normal3<T>::length() const {
        return std::sqrt(lengthSquared());
    }

    template <typename T>
    inline T Normal3<T>::operator[](std::size_t i) const {
        assert(i < 3);
        switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: assert(false);
        }
    }

    template <typename T>
    inline T& Normal3<T>::operator[](std::size_t i) {
        assert(i < 3);
        switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: assert(false);
        }
    }

    template <typename T>
    Normal3<T>& Normal3<T>::operator-=(const Normal3<T>& n) {
        assert(!n.hasNaNs());
        x -= n.x;
        y -= n.y;
        z -= n.z;
        return *this;
    }

    template <typename T>
    Normal3<T>& Normal3<T>::operator+=(const Normal3<T>& n) {
        assert(!n.hasNaNs());
        x += n.x;
        y += n.y;
        z += n.z;
        return *this;
    }

    template <typename T>
    template <typename U>
    Normal3<T>& Normal3<T>::operator*=(U f) {
        static_assert(std::is_arithmetic_v<U>, "Cannot scale a normal with non-arithmetic type");
        assert(!isNaN(f));

        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    template <typename T>
    template <typename U>
    Normal3<T>& Normal3<T>::operator/=(U f) {
        static_assert(std::is_arithmetic_v<U>, "Cannot divide a normal with non-arithmetic type");
        assert(f != 0);

        Float inv = Float{1} / f;
        return *this *= inv;
    }

    template <typename T>
    inline bool operator==(const Normal3<T>& n1, const Normal3<T>& n2) noexcept {
        return n1.x == n2.x && n1.y == n2.y && n1.z == n2.z;
    }

    template <typename T>
    inline bool operator!=(const Normal3<T>& n1, const Normal3<T>& n2) noexcept {
        return !(n1 == n2);
    }

    template <typename T, typename U>
    inline Normal3<T> operator/(const Normal3<T>& n, U f) {
        auto temp = n;
        temp /= f;
        return temp;
    }

    template <typename T>
    inline Normal3<T> operator-(const Normal3<T>& n) {
        return {-n.x, -n.y, -n.z};
    }

    template <typename T>
    inline Normal3<T> operator+(const Normal3<T>& n1, const Normal3<T>& n2) {
        auto temp = n1;
        temp += n2;
        return temp;
    }

    template <typename T>
    inline Normal3<T> operator-(const Normal3<T>& n1, const Normal3<T>& n2) {
        auto temp = n1;
        temp -= n2;
        return temp;
    }

    template <typename T, typename U>
    inline Normal3<T> operator*(U f, const Normal3<T>& n) {
        auto temp = n;
        temp *= f;
        return temp;
    }

    template <typename T>
    inline Normal3<T> normalize(const Normal3<T>& n) {
        return n / n.length();
    }

    template <typename T>
    inline Normal3<T> abs(const Normal3<T>& n) {
        return {std::abs(n.x), std::abs(n.y), std::abs(n.z)};
    }

    template <typename T>
    inline T dot(const Normal3<T>& n, const Vector3<T>& v) {
        assert(!n.hasNaNs());
        assert(!v.hasNaNs());
        return n.x * v.x + n.y * v.y + n.z * v.z;
    }

    template <typename T>
    inline T dot(const Vector3<T>& v, const Normal3<T>& n) {
        return dot(n, v);
    }

    template <typename T>
    inline T dot(const Normal3<T>& n1, const Normal3<T>& n2) {
        assert(!n1.hasNaNs());
        assert(!n2.hasNaNs());
        return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
    }

    template <typename T>
    inline T absDot(const Normal3<T>& n, const Vector3<T>& v) {
        return std::abs(dot(n, v));
    }

    template <typename T>
    inline T absDot(const Vector3<T>& v, const Normal3<T>& n) {
        return absDot(n, v);
    }

    template <typename T>
    inline T absDot(const Normal3<T>& n1, const Normal3<T>& n2) {
        return std::abs(dot(n1, n2));
    }

    template <typename T, typename F>
    inline T faceforward(const T& u, const F& v) {
        return (dot(u, v) < 0.f) ? -u : u;
    }
} //namespace idragnev::pbrt 