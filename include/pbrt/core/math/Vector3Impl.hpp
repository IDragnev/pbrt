#pragma once

namespace idragnev::pbrt::math {
    template <typename T>
    inline Vector3<T>::Vector3(T x, T y, T z) : x{x}
                                              , y{y}
                                              , z{z} {
        assert(!hasNaNs());
    }

    template <typename T>
    inline bool Vector3<T>::hasNaNs() const noexcept {
        return isNaN(x) || isNaN(y) || isNaN(z);
    }

    template <typename T>
    inline Vector3<T>::Vector3(const Point3<T>& p) : Vector3{p.x, p.y, p.z} {}

    template <typename T>
    inline Vector3<T>::Vector3(const Normal3<T>& n) : Vector3{n.x, n.y, n.z} {}

    template <typename T>
    inline Float Vector3<T>::lengthSquared() const noexcept {
        return x * x + y * y + z * z;
    }

    template <typename T>
    inline Float Vector3<T>::length() const {
        return std::sqrt(lengthSquared());
    }
    
    template <typename T>
    T Vector3<T>::operator[](const std::size_t i) const {
        assert(i < 3);
        if (i == 0) {
            return x;
        }
        else if (i == 1) {
            return y;
        }
        
        return z; 
    }

    template <typename T>
    T& Vector3<T>::operator[](const std::size_t i) {
        assert(i < 3);
        if (i == 0) {
            return x;
        }
        else if (i == 1) {
            return y;
        }

        return z;
    }

    template <typename T>
    Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& v) {
        assert(!v.hasNaNs());
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    template <typename T>
    Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& v) {
        assert(!v.hasNaNs());
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    template <typename T>
    template <typename U>
    Vector3<T>& Vector3<T>::operator*=(U f) {
        static_assert(std::is_arithmetic_v<U>,
                      "Cannot scale a vector with non-arithmetic type");
        assert(!isNaN(f));

        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    template <typename T>
    template <typename U>
    Vector3<T>& Vector3<T>::operator/=(U f) {
        static_assert(std::is_arithmetic_v<U>,
                      "Cannot divide a vector with non-arithmetic type");
        assert(f != 0);

        Float inv = Float{1} / f;
        return *this *= inv;
    }

    template <typename T>
    Vector3<T> operator+(const Vector3<T>& u, const Vector3<T>& v) {
        auto temp = u;
        temp += v;
        return temp;
    }

    template <typename T>
    Vector3<T> operator-(const Vector3<T>& u, const Vector3<T>& v) {
        auto temp = u;
        temp -= v;
        return temp;
    }

    template <typename T>
    inline bool operator==(const Vector3<T>& u, const Vector3<T>& v) noexcept {
        return u.x == v.x && u.y == v.y && u.z == v.z;
    }

    template <typename T>
    inline bool operator!=(const Vector3<T>& u, const Vector3<T>& v) noexcept {
        return !(u == v);
    }

    template <typename T, typename U>
    Vector3<T> operator*(const U f, const Vector3<T>& v) {
        auto temp = v;
        temp *= f;
        return temp;
    }

    template <typename T, typename U>
    Vector3<T> operator/(const Vector3<T>& v, const U f) {
        assert(f != 0);
        auto temp = v;
        temp /= f;
        return temp;
    }

    template <typename T>
    inline Vector3<T> operator-(const Vector3<T>& v) {
        return Vector3<T>(-v.x, -v.y, -v.z);
    }

    template <typename T>
    inline Float dot(const Vector3<T>& u, const Vector3<T>& v) {
        assert(!u.hasNaNs() && !v.hasNaNs());
        return u.x * v.x + u.y * v.y + u.z * v.z;
    }

    template <typename T>
    inline Float absDot(const Vector3<T>& u, const Vector3<T>& v) {
        return std::abs(Dot(u, v));
    }

    template <typename T>
    inline Vector3<T> normalize(const Vector3<T>& v) noexcept {
        return v / v.length();
    }

    template <typename T>
    inline Vector3<T> abs(const Vector3<T>& v) {
        return {std::abs(v.x), std::abs(v.y), std::abs(v.z)};
    }

    template <typename T>
    inline T minComponent(const Vector3<T>& v) noexcept {
        return std::min(v.x, std::min(v.y, v.z));
    }

    template <typename T>
    inline T maxComponent(const Vector3<T>& v) noexcept {
        return std::max(v.x, std::max(v.y, v.z));
    }

    template <typename T>
    std::size_t maxDimension(const Vector3<T>& v) noexcept {
        if (v.x > v.y) {
            return v.x > v.z ? 0 : 2;
        }
        else {
            return v.y > v.z ? 1 : 2;
        }
    }

    template <typename T>
    Vector3<T> min(const Vector3<T>& u, const Vector3<T>& v) {
        return {std::min(u.x, v.x), std::min(u.y, v.y), std::min(u.z, v.z)};
    }

    template <typename T>
    Vector3<T> max(const Vector3<T>& u, const Vector3<T>& v) {
        return {std::max(u.x, v.x), std::max(u.y, v.y), std::max(u.z, v.z)};
    }

    template <typename T>
    Vector3<T>
    permute(const Vector3<T>& v, std::size_t x, std::size_t y, std::size_t z) {
        return {v[x], v[y], v[z]};
    }

    template <typename T>
    Basis3<T> coordinateSystem(const Vector3<T>& u) {
        const auto v = [&u] {
            return std::abs(u.x) > std::abs(u.y)
                       ? (Vector3<T>{-u.z, 0, u.x} /
                          std::sqrt(u.x * u.x + u.z * u.z))
                       : (Vector3<T>{0, u.z, -u.y} /
                          std::sqrt(u.y * u.y + u.z * u.z));
        }();
        const auto w = cross(u, v);
        return {u, v, w};
    }

    template <typename T>
    Vector3<T> cross(const Vector3<T>& u, const Vector3<T>& v) {
        assert(!u.hasNaNs());
        assert(!v.hasNaNs());

        const double ux = u.x;
        const double uy = u.y;
        const double uz = u.z;
        const double vx = v.x;
        const double vy = v.y;
        const double vz = v.z;

        return Vector3<T>(static_cast<T>((uy * vz) - (uz * vy)),
                          static_cast<T>((uz * vx) - (ux * vz)),
                          static_cast<T>((ux * vy) - (uy * vx)));
    }

    template <typename T>
    inline Vector3<T> cross(const Vector3<T>& u, const Normal3<T>& n) {
        assert(!u.hasNaNs());
        assert(!n.hasNaNs());
        return cross(u, Vector3<T>{n});
    }

    template <typename T>
    inline Vector3<T> cross(const Normal3<T>& n, const Vector3<T>& v) {
        return cross(Vector3<T>{n}, v);
    }
} // namespace idragnev::pbrt::math