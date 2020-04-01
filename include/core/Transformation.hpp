#pragma once 

#include "core.hpp"
#include "Matrix4x4.hpp"

namespace idragnev::pbrt {
    class Transformation 
    {
    public:
        Transformation() = default;

        explicit Transformation(const Float mat[4][4])
            : m{mat}
            , mInverse{inverse(m)}
        {
        }

        explicit Transformation(const Matrix4x4& m)
            : m{m}
            , mInverse{inverse(m)}
        {
        }

        Transformation(const Matrix4x4& m, const Matrix4x4& mInv) noexcept
            : m{m}
            , mInverse{mInv}
        {
        }

        bool isIdentity() const noexcept {
            return m == Matrix4x4::identity();
        }

        const Matrix4x4& matrix() const noexcept { return m; }
        const Matrix4x4& inverseMatrix() const noexcept { return mInverse; }

        bool hasScale() const noexcept;
        bool swapsHandednes() const noexcept;

        template <typename T>
        Point3<T> operator()(const Point3<T>& p) const;
        
        template <typename T>
        Vector3<T> operator()(const Vector3<T>& v) const;
        
        template <typename T>
        Normal3<T> operator()(const Normal3<T>&) const;
        
        Ray operator()(const Ray& r) const;
        
        RayDifferential operator()(const RayDifferential& r) const;
        
        Bounds3f operator()(const Bounds3f& b) const;

    private:
        Matrix4x4 m;
        Matrix4x4 mInverse;
    };

    inline Transformation inverse(const Transformation& t) noexcept {
        return Transformation{ 
            t.inverseMatrix(),
            t.matrix()
        };
    }

    inline Transformation transpose(const Transformation& t) noexcept {
        return Transformation{
            transpose(t.matrix()),
            transpose(t.inverseMatrix())
        };
    }

    inline Transformation operator*(const Transformation& lhs, const Transformation& rhs) noexcept {
        return Transformation{
            lhs.matrix() * rhs.matrix(),
            rhs.inverseMatrix() * lhs.inverseMatrix()
        };
    }

    inline bool operator==(const Transformation& lhs, const Transformation& rhs) noexcept {
        return lhs.matrix() == rhs.matrix() && 
               lhs.inverseMatrix() == rhs.inverseMatrix();
    }

    inline bool operator!=(const Transformation& lhs, const Transformation& rhs) noexcept {
        return !(lhs == rhs);
    }

    Transformation translation(const Vector3f& delta) noexcept;
    Transformation scaling(const Float x, const Float y, const Float z) noexcept;
    Transformation xRotation(const Float theta) noexcept;
    Transformation yRotation(const Float theta) noexcept;
    Transformation zRotation(const Float theta) noexcept;
    Transformation rotation(const Float theta, const Vector3f& axis) noexcept;
    Transformation lookAt(const Point3f& origin, const Point3f& look, const Vector3f& up) noexcept;
} //namespace idragnev::pbrt

#include "TransformationImpl.hpp"