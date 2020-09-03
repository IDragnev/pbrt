#pragma once

#include "core/Vector3.hpp"
#include "core/Quaternion.hpp"
#include "core/Matrix4x4.hpp"

namespace idragnev::pbrt {
    struct TRS
    {
        Vector3f T;
        Quaternion R;
        Matrix4x4 S;
    };

    TRS decompose(const Matrix4x4& m);

    namespace detail {
        Matrix4x4 removeTranslation(const Matrix4x4& matrix) noexcept;
        Matrix4x4 extractRotation(const Matrix4x4& SR);
        Matrix4x4 computeRNext(const Matrix4x4& R);
        Float computeNormDifference(const Matrix4x4& R, const Matrix4x4& RNext);
    } // namespace detail
} // namespace idragnev::pbrt