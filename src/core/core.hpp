#pragma once

#include <limits>
#include <cmath>

namespace idragnev::pbrt {
    using Float = float;

    inline constexpr auto Infinity = std::numeric_limits<Float>::infinity();

    template <typename T>
    class Vector2;  
    template <typename T>
    class Vector3;

    template <typename T>
    class Point2;
    template <typename T>
    class Point3;

    template <typename T>
    class Normal3;

    template <typename T>
    class Bounds2;
    template <typename T>
    class Bounds3;

    using Vector2f = Vector2<Float>;
    using Vector2i = Vector2<int>;
    using Vector3f = Vector3<Float>;
    using Vector3i = Vector3<int>;

    using Point2f = Point2<Float>;
    using Point2i = Point2<int>;
    using Point3f = Point3<Float>;
    using Point3i = Point3<int>;

    using Normal3f = Normal3<Float>;

    using Bounds2f = Bounds2<Float>;
    using Bounds3f = Bounds3<Float>;

    template <
        typename T,
        typename = std::enable_if_t<std::is_floating_point_v<T>>
    >
    inline bool isNaN(const T x) {
        return std::isnan(x);
    }

    template <
        typename T,
        typename = std::enable_if_t<std::is_integral_v<T>>
    >
    inline bool isNaN(const T&) {
        return false;
    }
}
