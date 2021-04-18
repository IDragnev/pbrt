#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/geometry/Ray.hpp"
#include "pbrt/core/math/Fwd.hpp"
#include "pbrt/core/math/Matrix4x4.hpp"

namespace idragnev::pbrt {
    struct RayWithErrorBound
    {
        Ray ray;
        Vector3f oError;
        Vector3f dError;
    };

    class Transformation
    {
    public:
        Transformation() = default;

        explicit Transformation(const Float mat[4][4])
            : m{mat}
            , mInverse{inverse(m)} {}

        explicit Transformation(const math::Matrix4x4& m)
            : m{m}
            , mInverse{inverse(m)} {}

        Transformation(const math::Matrix4x4& m,
                       const math::Matrix4x4& mInv) noexcept
            : m{m}
            , mInverse{mInv} {}

        bool isIdentity() const noexcept {
            return m == math::Matrix4x4::identity();
        }

        const math::Matrix4x4& matrix() const noexcept { return m; }
        const math::Matrix4x4& inverseMatrix() const noexcept {
            return mInverse;
        }

        bool hasScale() const noexcept;
        bool swapsHandedness() const noexcept;

        template <typename T>
        math::Point3<T> operator()(const math::Point3<T>& p) const;

        template <typename T>
        math::Vector3<T> operator()(const math::Vector3<T>& v) const;

        template <typename T>
        math::Normal3<T> operator()(const math::Normal3<T>&) const;

        Ray operator()(const Ray& r) const;

        RayDifferential operator()(const RayDifferential& r) const;

        Bounds3f operator()(const Bounds3f& b) const;

        SurfaceInteraction operator()(const SurfaceInteraction& si) const;

        RayWithErrorBound transformWithErrBound(const Ray& r) const;
        RayWithErrorBound transformWithErrBound(const Ray& r,
                                                const Vector3f& oErrorIn,
                                                const Vector3f& dErrorIn) const;

    private:
        math::Matrix4x4 m;
        math::Matrix4x4 mInverse;
    };

    inline Transformation inverse(const Transformation& t) noexcept {
        return Transformation{t.inverseMatrix(), t.matrix()};
    }

    inline Transformation transpose(const Transformation& t) noexcept {
        return Transformation{transpose(t.matrix()),
                              transpose(t.inverseMatrix())};
    }

    inline Transformation operator*(const Transformation& lhs,
                                    const Transformation& rhs) noexcept {
        return Transformation{lhs.matrix() * rhs.matrix(),
                              rhs.inverseMatrix() * lhs.inverseMatrix()};
    }

    inline bool operator==(const Transformation& lhs,
                           const Transformation& rhs) noexcept {
        return lhs.matrix() == rhs.matrix() &&
               lhs.inverseMatrix() == rhs.inverseMatrix();
    }

    inline bool operator!=(const Transformation& lhs,
                           const Transformation& rhs) noexcept {
        return !(lhs == rhs);
    }

    Transformation translation(const Vector3f& delta) noexcept;
    Transformation
    scaling(const Float x, const Float y, const Float z) noexcept;
    Transformation xRotation(const Float theta) noexcept;
    Transformation yRotation(const Float theta) noexcept;
    Transformation zRotation(const Float theta) noexcept;
    Transformation rotation(const Float theta, const Vector3f& axis) noexcept;
    Transformation lookAt(const Point3f& origin,
                          const Point3f& look,
                          const Vector3f& up) noexcept;
    Transformation
    perspectiveTransform(const Float fov, const Float zNear, const Float zFar);
    inline Transformation orthographicTransform(const Float zNear,
                                                const Float zFar) {
        return scaling(1.f, 1.f, 1.f / (zFar - zNear)) *
               translation(Vector3f(0.f, 0.f, -zNear));
    }
} // namespace idragnev::pbrt

#include "pbrt/core/transformations/TransformationImpl.hpp"