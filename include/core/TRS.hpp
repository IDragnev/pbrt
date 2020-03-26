#pragma once

#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "Matrix4x4.hpp"

namespace idragnev::pbrt {
    struct TRS
    {
        Vector3f translation;
        Quaternion rotation;
        Matrix4x4 scaling;
    };

    TRS decompose(const Matrix4x4& m);

    namespace detail {
        Matrix4x4 removeTranslation(const Matrix4x4& matrix) noexcept;
        Matrix4x4 extractRotation(const Matrix4x4& SR);
        Matrix4x4 computeRNext(const Matrix4x4& R);
        Float computeNormDifference(const Matrix4x4& R, const Matrix4x4& RNext);
    }
} //namespace idragnev::pbrt