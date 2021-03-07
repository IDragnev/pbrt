#include "pbrt/core/transformations/Transformation.hpp"
#include "pbrt/core/math/Vector3.hpp"
#include "pbrt/core/math/Point3.hpp"
#include "pbrt/core/geometry/Bounds3.hpp"
#include "pbrt/core/SurfaceInteraction.hpp"

namespace idragnev::pbrt {
    bool Transformation::hasScale() const noexcept {
        const auto scales = [&transform = *this](const Vector3f& v) {
            const auto len = transform(v).lengthSquared();
            return len < 0.999f || len > 1.001f;
        };

        // clang-format off
        return scales({ 1.f, 0.f, 0.f }) ||
               scales({ 0.f, 1.f, 0.f }) ||
               scales({ 0.f, 0.f, 1.f });
        // clang-format on
    }

    bool Transformation::swapsHandedness() const noexcept {
        // clang-format off
        const auto det =
            m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) -
            m.m[0][1] * (m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0]) +
            m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]);
        // clang-format on

        return det < 0.f;
    }

    Bounds3f Transformation::operator()(const Bounds3f& bounds) const {
        const auto scaleColumn = [&matrix = m.m](const std::size_t c,
                                                 const auto s) {
            return Point3f(matrix[0][c] * s,
                           matrix[1][c] * s,
                           matrix[2][c] * s);
        };

        const auto xMin = scaleColumn(0, bounds.min.x);
        const auto xMax = scaleColumn(0, bounds.max.x);
        const auto yMin = scaleColumn(1, bounds.min.y);
        const auto yMax = scaleColumn(1, bounds.max.y);
        const auto zMin = scaleColumn(2, bounds.min.z);
        const auto zMax = scaleColumn(2, bounds.max.z);
        const auto translation = Vector3f(m.m[0][3], m.m[1][3], m.m[2][3]);

        Bounds3f result;
        result.min =
            min(xMin, xMax) + min(yMin, yMax) + min(zMin, zMax) + translation;
        result.max =
            max(xMin, xMax) + max(yMin, yMax) + max(zMin, zMax) + translation;

        return result;
    }

    SurfaceInteraction
    Transformation::operator()([[maybe_unused]] const SurfaceInteraction& si) const {
        // TODO
        return {};
    }

    RayWithErrorBound
    Transformation::transformWithErrBound([[maybe_unused]] const Ray& r) const {
        // TODO
        return {};
    }

    RayWithErrorBound
    Transformation::transformWithErrBound([[maybe_unused]] const Ray& r,
                                          [[maybe_unused]] const Vector3f& oErrorIn,
                                          [[maybe_unused]] const Vector3f& dErrorIn) const {
        // TODO
        return {};
    }

    Transformation translation(const Vector3f& delta) noexcept {
        // clang-format off
        const math::Matrix4x4 m{ 
            1.f, 0.f, 0.f, delta.x,
            0.f, 1.f, 0.f, delta.y,
            0.f, 0.f, 1.f, delta.z,
            0.f, 0.f, 0.f, 1.f
        };
        const math::Matrix4x4 mInverse{
            1.f, 0.f, 0.f, -delta.x,
            0.f, 1.f, 0.f, -delta.y,
            0.f, 0.f, 1.f, -delta.z,
            0.f, 0.f, 0.f, 1.f
        };
        // clang-format on

        return Transformation{m, mInverse};
    }

    Transformation
    scaling(const Float x, const Float y, const Float z) noexcept {
        // clang-format off
        const math::Matrix4x4 m{
            x,   0.f, 0.f, 0.f,
            0.f, y,   0.f, 0.f,
            0.f, 0.f, z,   0.f,
            0.f, 0.f, 0.f, 1.f
        };
        const math::Matrix4x4 mInverse{
            1.f / x,  0.f,      0.f,      0.f,
            0.f,      1.f / y,  0.f,      0.f,
            0.f,      0.f,      1.f / z,  0.f,
            0.f,      0.f,      0.f,      1.f
        };
        // clang-format on

        return Transformation{m, mInverse};
    }

    Transformation xRotation(const Float theta) noexcept {
        const auto sinTh = std::sin(math::toRadians(theta));
        const auto cosTh = std::cos(math::toRadians(theta));

        // clang-format off
        const math::Matrix4x4 m{
            1.f, 0.f,   0.f,    0.f, 
            0.f, cosTh, -sinTh, 0.f,
            0.f, sinTh, cosTh,  0.f,
            0.f, 0.f,   0.f,    1.f
        };
        // clang-format on

        return Transformation{m, transpose(m)};
    }

    Transformation yRotation(const Float theta) noexcept {
        const auto sinTh = std::sin(math::toRadians(theta));
        const auto cosTh = std::cos(math::toRadians(theta));

        // clang-format off
        const math::Matrix4x4 m{
            cosTh,   0.f,  sinTh,   0.f,
            0.f,     1.f,  0.f,     0.f,
            -sinTh,  0.f,  cosTh,   0.f,
            0.f,     0.f,  0.f,     1.f
        };
        // clang-format on

        return Transformation{m, transpose(m)};
    }

    Transformation zRotation(const Float theta) noexcept {
        const auto sinTh = std::sin(math::toRadians(theta));
        const auto cosTh = std::cos(math::toRadians(theta));

        // clang-format off
        const math::Matrix4x4 m{
            cosTh, -sinTh, 0.f, 0.f,
            sinTh, cosTh,  0.f, 0.f,
            0.f,     0.f,  1.f, 0.f,
            0.f,     0.f,  0.f, 1.f
        };
        // clang-format on

        return Transformation{m, transpose(m)};
    }

    Transformation rotation(const Float theta,
                            const Vector3f& rotationAxis) noexcept {
        const auto axis = normalize(rotationAxis);
        const Float sinTheta = std::sin(math::toRadians(theta));
        const Float cosTheta = std::cos(math::toRadians(theta));

        math::Matrix4x4 matrix;

        // clang-format off
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
        // clang-format on

        return Transformation{matrix, transpose(matrix)};
    }

    Transformation lookAt(const Point3f& origin,
                          const Point3f& look,
                          const Vector3f& up) noexcept {
        const auto direction = normalize(look - origin);
        const auto normalizedUp = normalize(up);
        const auto upDirCross = cross(normalizedUp, direction);

        if (const auto upAndDirectionHaveSameDirection =
                upDirCross.length() == 0.f;
            upAndDirectionHaveSameDirection)
        {
            return Transformation{};
        }

        const auto right = normalize(upDirCross);
        const auto newUp = cross(direction, right);

        // clang-format off
        const auto cameraToWorld = math::Matrix4x4{
            right.x,  newUp.x,  direction.x,  origin.x,
            right.y,  newUp.y,  direction.y,  origin.y,
            right.z,  newUp.z,  direction.z,  origin.z,
            0.f,      0.f,      0.f,          1.f
        };
        // clang-format on

        return Transformation{inverse(cameraToWorld), cameraToWorld};
    }
} // namespace idragnev::pbrt