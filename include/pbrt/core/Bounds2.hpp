#pragma once

#include "math/Point2.hpp"
#include "math/Vector2.hpp"
#include "BoundingSphere.hpp"
#include "Bounds2Iterator.hpp"

namespace idragnev::pbrt {
    template <typename T>
    class Bounds2
    {
        static_assert(std::is_arithmetic_v<T>,
                      "Cannot instantiate Bounds2 with non-arithmetic type");

    public:
        using PointType = math::Point2<T>;

        Bounds2();
        explicit Bounds2(const math::Point2<T>& p);
        Bounds2(const math::Point2<T>& p1, const math::Point2<T>& p2);

        template <typename U>
        explicit operator Bounds2<U>() const;

        math::Vector2<T> diagonal() const;

        T area() const;

        std::size_t maximumExtent() const;

        math::Point2<T>& operator[](std::size_t i);
        const math::Point2<T>& operator[](std::size_t i) const;

        math::Vector2<T> offset(const math::Point2<T>& p) const;

        BoundingSphere2<T> boundingSphere() const;

    public:
        math::Point2<T> min;
        math::Point2<T> max;
    };

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    inline Bounds2Iterator<T> begin(const Bounds2<T>& bounds);

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    Bounds2Iterator<T> end(const Bounds2<T>& bounds);

    template <typename T>
    math::Point2<T> lerp(const Bounds2<T>& bounds, const Point2f& t);

    template <typename T>
    bool operator==(const Bounds2<T>& a, const Bounds2<T>& b);

    template <typename T>
    bool operator!=(const Bounds2<T>& a, const Bounds2<T>& b);

    template <typename T>
    Bounds2<T> unionOf(const Bounds2<T>& b, const math::Point2<T>& p);

    template <typename T>
    Bounds2<T> unionOf(const Bounds2<T>& a, const Bounds2<T>& b);

    template <typename T>
    Bounds2<T> intersectionOf(const Bounds2<T>& b1, const Bounds2<T>& b2);

    template <typename T>
    bool overlap(const Bounds2<T>& ba, const Bounds2<T>& bb) noexcept;

    template <typename T>
    bool inside(const math::Point2<T>& p, const Bounds2<T>& bounds) noexcept;

    template <typename T>
    bool insideExclusive(const math::Point2<T>& p, const Bounds2<T>& bounds) noexcept;

    template <typename T, typename U>
    Bounds2<T> expand(const Bounds2<T>& bounds, U delta);
} // namespace idragnev::pbrt

#include "Bounds2Impl.hpp"