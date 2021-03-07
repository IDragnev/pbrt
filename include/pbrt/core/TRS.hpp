#pragma once

#include "math/Vector3.hpp"
#include "math/Matrix4x4.hpp"
#include "Quaternion.hpp"

namespace idragnev::pbrt {
    struct TRS
    {
        Vector3f T;
        Quaternion R;
        math::Matrix4x4 S;
    };

    TRS decompose(const math::Matrix4x4& m);
} // namespace idragnev::pbrt