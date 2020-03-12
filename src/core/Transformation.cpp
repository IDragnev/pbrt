#include "Transformation.hpp"
#include "Vector3.hpp"

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
} //namespace idragnev::pbrt