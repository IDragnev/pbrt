#pragma once

#include "core.hpp"
#include "Point3.hpp"
#include "BoundingSphere.hpp"
#include "Optional.hpp"

namespace idragnev::pbrt {
    template <typename T>
    class Bounds3
    {
        static_assert(std::is_arithmetic_v<T>,
                      "Cannot instantiate Bounds3 with non-arithmetic type");

    public:
        using PointType = Point3<T>;

        Bounds3();
        explicit Bounds3(const Point3<T>& p);
        Bounds3(const Point3<T>& p1, const Point3<T>& p2);

        template <typename U>
        explicit operator Bounds3<U>() const;

        Optional<Intervalf> intersectP(const Ray& ray) const noexcept;
        bool intersectP(const Ray& ray,
                        const Vector3f& invDir,
                        const std::size_t dirIsNeg[3]) const noexcept;

        Vector3<T> diagonal() const;

        T surfaceArea() const;
        T volume() const;

        std::size_t maximumExtent() const;

        Vector3<T> offset(const Point3<T>& p) const;

        Point3<T>& operator[](std::size_t i);
        const Point3<T>& operator[](std::size_t i) const;

        BoundingSphere3<T> boundingSphere() const;

        Point3<T> corner(std::size_t corner) const;

    public:
        Point3<T> min;
        Point3<T> max;
    };

    template <typename T>
    Point3<T> lerp(const Bounds3<T>& bounds, const Point3f& t);

    template <typename T>
    bool operator==(const Bounds3<T>& a, const Bounds3<T>& b);

    template <typename T>
    bool operator!=(const Bounds3<T>& a, const Bounds3<T>& b);

    template <typename T>
    Bounds3<T> unionOf(const Bounds3<T>& b, const Point3<T>& p);

    template <typename T>
    Bounds3<T> unionOf(const Bounds3<T>& a, const Bounds3<T>& b);

    template <typename T>
    Bounds3<T> intersectionOf(const Bounds3<T>& b1, const Bounds3<T>& b2);

    template <typename T>
    bool overlap(const Bounds3<T>& ba, const Bounds3<T>& bb) noexcept;

    template <typename T>
    bool inside(const Point3<T>& p, const Bounds3<T>& bounds) noexcept;

    template <typename T>
    bool insideExclusive(const Point3<T>& p, const Bounds3<T>& bounds) noexcept;

    template <typename T, typename U>
    Bounds3<T> expand(const Bounds3<T>& bounds, U delta);

    template <typename T, typename U>
    Float distanceSquared(const Point3<T>& p, const Bounds3<U>& b);

    template <typename T, typename U>
    Float distance(const Point3<T>& p, const Bounds3<U>& b);
} // namespace idragnev::pbrt

#include "Bounds3Impl.hpp"