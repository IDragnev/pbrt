#pragma once

#include "Point3.hpp"
#include "Vector3.hpp"

namespace idragnev::pbrt {
    template <typename T>
    Point3<T> Transformation::operator()(const Point3<T>& p) const {
        const auto& matrix = m.m;
        const auto x = matrix[0][0] * p.x + matrix[0][1] * p.y + matrix[0][2] * p.z + matrix[0][3];
        const auto y = matrix[1][0] * p.x + matrix[1][1] * p.y + matrix[1][2] * p.z + matrix[1][3];
        const auto z = matrix[2][0] * p.x + matrix[2][1] * p.y + matrix[2][2] * p.z + matrix[2][3];
        const auto w = matrix[3][0] * p.x + matrix[3][1] * p.y + matrix[3][2] * p.z + matrix[3][3];
        return (w == 1) ? Point3<T>(x, y, z) : Point3<T>(x, y, z) / w;
    }

	template <typename T> 
    Vector3<T> Transformation::operator()(const Vector3<T>& v) const {
        const auto& matrix = m.m;
		return Vector3<T>(
			matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2] * v.z,
			matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2] * v.z,
			matrix[2][0] * v.x + matrix[2][1] * v.y + matrix[2][2] * v.z
        );
    }
} //namespace idragnev::pbrt