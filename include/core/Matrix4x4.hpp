#pragma once

#include "core.hpp"

namespace idragnev::pbrt {
    struct Matrix4x4 {
        Matrix4x4(const Float mat[4][4]);
        
        Matrix4x4() noexcept 
            : Matrix4x4{
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            }
        {
        }
        
        Matrix4x4::Matrix4x4(
            Float t00, Float t01, Float t02, Float t03,
            Float t10, Float t11, Float t12, Float t13,
            Float t20, Float t21, Float t22, Float t23,
            Float t30, Float t31, Float t32, Float t33
        ) noexcept
            : m{
                t00, t01, t02, t03,
                t10, t11, t12, t13,
                t20, t21, t22, t23,
                t30, t31, t32, t33
            }
        {
        }

        static const Matrix4x4& identity() noexcept;

        Float m[4][4];
    };

    Matrix4x4 inverse(const Matrix4x4& m);
    Matrix4x4 transpose(const Matrix4x4&) noexcept;
    
    Matrix4x4 mul(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept;
    
    inline Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
        return mul(lhs, rhs);
    }

    bool operator==(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept;

    inline bool operator!=(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
        return !(lhs == rhs);
    }
} //namespace idragnev::pbrt