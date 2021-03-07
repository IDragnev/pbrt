#pragma once

#include "math/Point3.hpp"
#include "math/Vector3.hpp"

namespace idragnev::pbrt {
    template <typename T>
    math::Point3<T> Transformation::operator()(const math::Point3<T>& p) const {
        const auto& matrix = m.m;

        // clang-format off
        const auto x = matrix[0][0] * p.x + matrix[0][1] * p.y + matrix[0][2] * p.z + matrix[0][3];
        const auto y = matrix[1][0] * p.x + matrix[1][1] * p.y + matrix[1][2] * p.z + matrix[1][3];
        const auto z = matrix[2][0] * p.x + matrix[2][1] * p.y + matrix[2][2] * p.z + matrix[2][3];
        const auto w = matrix[3][0] * p.x + matrix[3][1] * p.y + matrix[3][2] * p.z + matrix[3][3];
        // clang-format on

        return (w == 1) ? math::Point3<T>(x, y, z)
                        : math::Point3<T>(x, y, z) / w;
    }

    template <typename T>
    math::Vector3<T> Transformation::operator()(const math::Vector3<T>& v) const {
        const auto& matrix = m.m;

        // clang-format off
	    return math::Vector3<T>(
		    matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2] * v.z,
		    matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2] * v.z,
		    matrix[2][0] * v.x + matrix[2][1] * v.y + matrix[2][2] * v.z
        );
        // clang-format on
    }

    template <typename T>
    math::Normal3<T> Transformation::operator()(const math::Normal3<T>& n) const {
        const auto& matrix = mInverse.m;

        // clang-format off
        return math::Normal3<T>(
            matrix[0][0] * n.x + matrix[1][0] * n.y + matrix[2][0] * n.z,
            matrix[0][1] * n.x + matrix[1][1] * n.y + matrix[2][1] * n.z,
            matrix[0][2] * n.x + matrix[1][2] * n.y + matrix[2][2] * n.z
        );
        // clang-format on
    }
} // namespace idragnev::pbrt