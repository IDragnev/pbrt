#include "Transformation.hpp"
#include "Vector3.hpp"
#include "Point3.hpp"

namespace idragnev::pbrt {
    bool Transformation::hasScale() const noexcept {
        const auto scales = [&transform = *this](const Vector3f& v) {
            const auto len = transform(v).lengthSquared();
            return len < 0.999f || len > 1.001f;
        };
        return scales({ 1.f, 0.f, 0.f }) ||
               scales({ 0.f, 1.f, 0.f }) ||
               scales({ 0.f, 0.f, 1.f });
    }

    Transformation translation(const Vector3f& delta) noexcept {
        const Matrix4x4 m{ 
            1.f, 0.f, 0.f, delta.x,
            0.f, 1.f, 0.f, delta.y,
            0.f, 0.f, 1.f, delta.z,
            0.f, 0.f, 0.f, 1.f
        };
        const Matrix4x4 mInverse{
            1.f, 0.f, 0.f, -delta.x,
            0.f, 1.f, 0.f, -delta.y,
            0.f, 0.f, 1.f, -delta.z,
            0.f, 0.f, 0.f, 1.f
        };
        return Transformation{m, mInverse};
    }

    Transformation scaling(const Float x, const Float y, const Float z) noexcept {
        const Matrix4x4 m{
            x,   0.f, 0.f, 0.f,
            0.f, y,   0.f, 0.f,
            0.f, 0.f, z,   0.f,
            0.f, 0.f, 0.f, 1.f
        };
        const Matrix4x4 mInverse{
            1.f / x,  0.f,      0.f,      0.f,
            0.f,      1.f / y,  0.f,      0.f,
            0.f,      0.f,      1.f / z,  0.f,
            0.f,      0.f,      0.f,      1.f
        };
        return Transformation{m, mInverse};
    }

    Transformation xRotation(const Float theta) noexcept {
        const auto sinTh = std::sin(toRadians(theta));
        const auto cosTh = std::cos(toRadians(theta));
        const Matrix4x4 m{
            1.f, 0.f,   0.f,    0.f, 
            0.f, cosTh, -sinTh, 0.f,
            0.f, sinTh, cosTh,  0.f,
            0.f, 0.f,   0.f,    1.f
        };
        return Transformation{m, transpose(m)};
    }

    Transformation yRotation(const Float theta) noexcept {
        const auto sinTh = std::sin(toRadians(theta));
        const auto cosTh = std::cos(toRadians(theta));
        const Matrix4x4 m{
            cosTh,   0.f,  sinTh,   0.f,
            0.f,     1.f,  0.f,     0.f,
            -sinTh,  0.f,  cosTh,   0.f,
            0.f,     0.f,  0.f,     1.f
        };
        return Transformation{m, transpose(m)};
    }

    Transformation zRotation(const Float theta) noexcept {
        const auto sinTh = std::sin(toRadians(theta));
        const auto cosTh = std::cos(toRadians(theta));
        const Matrix4x4 m{ 
            cosTh, -sinTh, 0.f, 0.f,
            sinTh, cosTh,  0.f, 0.f,
            0.f,     0.f,  1.f, 0.f,
            0.f,     0.f,  0.f, 1.f
        };
        return Transformation{m, transpose(m)};
    }

    Transformation rotation(const Float theta, const Vector3f& rotationAxis) noexcept {
        const auto axis = normalize(rotationAxis);
        const Float sinTheta = std::sin(toRadians(theta));
        const Float cosTheta = std::cos(toRadians(theta));
        
        Matrix4x4 matrix;
        
        matrix.m[0][0] = axis.x * axis.x + (1.f - axis.x * axis.x) * cosTheta;
        matrix.m[0][1] = axis.x * axis.y * (1.f - cosTheta) - axis.z * sinTheta;
        matrix.m[0][2] = axis.x * axis.z * (1.f - cosTheta) + axis.y * sinTheta;
        matrix.m[0][3] = 0.f;

        matrix.m[1][0] = axis.x * axis.y * (1.f - cosTheta) + axis.z * sinTheta;
        matrix.m[1][1] = axis.y * axis.y + (1.f - axis.y * axis.y) * cosTheta;
        matrix.m[1][2] = axis.y * axis.z * (1.f - cosTheta) - axis.x * sinTheta;
        matrix.m[1][3] = 0.f;

        matrix.m[2][0] = axis.x * axis.z * (1.f - cosTheta) - axis.y * sinTheta;
        matrix.m[2][1] = axis.y * axis.z * (1.f - cosTheta) + axis.x * sinTheta;
        matrix.m[2][2] = axis.z * axis.z + (1.f - axis.z * axis.z) * cosTheta;
        matrix.m[2][3] = 0.f;

        return Transformation{matrix, transpose(matrix)};
    }

    Transformation lookAt(const Point3f& origin, const Point3f& look, const Vector3f& up) {
        const auto direction = normalize(look - origin);
        const auto normalizedUp = normalize(up);
        const auto upDirCross = cross(normalizedUp, direction);
        
        if (const auto upAndDirectionHaveSameDirection = upDirCross.length() == 0.f;
            upAndDirectionHaveSameDirection) {
            return Transformation{};
        }

        const auto right = normalize(upDirCross);
        const auto newUp = cross(direction, right);

        const auto cameraToWorld = Matrix4x4{
            right.x,  newUp.x,  direction.x,  origin.x,
            right.y,  newUp.y,  direction.y,  origin.y,
            right.z,  newUp.z,  direction.z,  origin.z,
            0.f,      0.f,      0.f,          1.f
        };

        return Transformation{
            inverse(cameraToWorld), 
            cameraToWorld
        };
    }
} //namespace idragnev::pbrt