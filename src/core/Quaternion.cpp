#include "Quaternion.hpp"
#include "Transformation.hpp"
#include "Matrix4x4.hpp"

namespace idragnev::pbrt {
    Quaternion::Quaternion(const Transformation& t)
    {
        const auto& matrix = t.matrix().m;

        const auto trace = matrix[0][0] + matrix[1][1] + matrix[2][2];
        if (trace > 0.f) {
            const auto s = std::sqrt(trace + 1.0f);
            w = s / 2.0f;
            
            const auto k = 0.5f / s;
            v.x = (matrix[2][1] - matrix[1][2]) * k;
            v.y = (matrix[0][2] - matrix[2][0]) * k;
            v.z = (matrix[1][0] - matrix[0][1]) * k;
        }
        else {
            const std::size_t next[3] = { 1, 2, 0 };
            const auto i = [&matrix] {
                auto i = 0;
                if (matrix[1][1] > matrix[0][0]) {
                    i = 1;
                }
                if (matrix[2][2] > matrix[i][i]) {
                    i = 2;
                }
                return i;
            }();
            const auto j = next[i];
            const auto k = next[j];
            
            auto s = std::sqrt((matrix[i][i] - (matrix[j][j] + matrix[k][k])) + 1.0f);
            
            Float q[3];
            q[i] = s * 0.5f;
            
            if (s != 0.f) {
                s = 0.5f / s;
            }
            
            q[j] = (matrix[j][i] + matrix[i][j]) * s;
            q[k] = (matrix[k][i] + matrix[i][k]) * s;

            w = (matrix[k][j] - matrix[j][k]) * s;            
            v = {q[0], q[1], q[2]};
        }
    }

    Quaternion& Quaternion::operator+=(const Quaternion& rhs) {
        v += rhs.v;
        w += rhs.w;
        return *this;
    }

    Quaternion& Quaternion::operator-=(const Quaternion& rhs) {
        v -= rhs.v;
        w -= rhs.w;
        return *this;
    }

    Quaternion& Quaternion::operator*=(const Float s) {
        v *= s;
        w *= s;
        return *this;
    }

    Quaternion& Quaternion::operator/=(const Float s) {
        v /= s;
        w /= s;
        return *this;
    }

    Transformation Quaternion::toTransformation() const noexcept {
        const auto xx = v.x * v.x;
        const auto yy = v.y * v.y;
        const auto zz = v.z * v.z;
        
        const auto xy = v.x * v.y;
        const auto xz = v.x * v.z;
        const auto yz = v.y * v.z;
        
        const auto wx = v.x * w;
        const auto wy = v.y * w; 
        const auto wz = v.z * w;

        const auto m = Matrix4x4{
            1.f - 2.f * (yy + zz),   2.f * (xy + wz),          2.f * (xz - wy),         0.f,
            2.f * (xy - wz),         1.f - 2.f * (xx + zz),    2.f * (yz + wx),         0.f,
            2.f * (xz + wy),         2.f * (yz - wx),          1.f - 2.f * (xx + yy),   0.f,
            0.f,                     0.f,                      0.f,                     1.f
        };

        return Transformation{ 
            transpose(m), //transpose since we are left-handed
            m
        };
    }

    Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) {
        auto temp = lhs;
        temp += rhs;
        return temp;
    }

    Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs) {
        auto temp = lhs;
        temp -= rhs;
        return temp;
    }

    Quaternion operator/(const Quaternion& q, const Float s) {
        auto temp = q;
        temp /= s;
        return temp;
    }

    Quaternion operator*(const Float s, const Quaternion& q) {
        auto temp = q;
        temp *= s;
        return temp;
    }
} //namespace idragnev::pbrt