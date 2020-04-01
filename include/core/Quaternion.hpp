#pragma once

#include "core.hpp"
#include "Vector3.hpp"

namespace idragnev::pbrt {
    class Quaternion
    {
    public:
        Quaternion() = default;
        Quaternion(const Vector3f& v, const Float w) : v{v}, w{w} { }
        explicit Quaternion(const Transformation& m);

        Quaternion& operator+=(const Quaternion& rhs);
        Quaternion& operator-=(const Quaternion& rhs);

        Quaternion& operator*=(const Float s);
        Quaternion& operator/=(const Float s);

        Transformation toTransformation() const noexcept;

    public:
        Vector3f v;
        Float w = 1.f;
    };

    inline Quaternion operator-(const Quaternion& q) {
        return Quaternion{-q.v, -q.w};
    }

    Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);
    Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs);
    
    Quaternion operator/(const Quaternion& q, const Float s);

    Quaternion operator*(const Float s, const Quaternion& q); 
    inline Quaternion operator*(const Quaternion& q, const Float s) {
        return s * q; 
    }
    
    Quaternion slerp(Float t, const Quaternion& a, const Quaternion& b);

    inline Float dot(const Quaternion& q1, const Quaternion& q2) {
        return dot(q1.v, q2.v) + q1.w * q2.w;
    }

    inline Quaternion normalize(const Quaternion& q) {
        return q / std::sqrt(dot(q, q));
    }
} //namespace idragnev::pbrt 