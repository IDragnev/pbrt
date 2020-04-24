#pragma once

#include <limits>
#include <cmath>

namespace idragnev::pbrt {

#ifdef RT_FLOAT_AS_DOUBLE
    using Float = double;
#else 
    using Float = float;
#endif

    namespace constants {
        inline constexpr auto Infinity = std::numeric_limits<Float>::infinity();
        inline constexpr Float MachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5;
        inline constexpr Float ShadowEpsilon = 0.0001f;
        inline constexpr Float Pi = 3.14159265358979323846;
        inline constexpr Float InvPi = 0.31830988618379067154;
        inline constexpr Float Inv2Pi = 0.15915494309189533577;
        inline constexpr Float Inv4Pi = 0.07957747154594766788;
        inline constexpr Float PiOver2 = 1.57079632679489661923;
        inline constexpr Float PiOver4 = 0.78539816339744830961;
        inline constexpr Float Sqrt2 = 1.41421356237309504880;
    } //namespace constants

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
    using Bounds2i = Bounds2<int>;
    using Bounds3f = Bounds3<Float>;

    class Ray;
    class RayDifferential;

    class Matrix4x4;
    class Transformation;

    class Quaternion;
    struct TRS;
    class AnimatedTransformation;

    class Interval;
    
    class Interaction;
    class SurfaceInteraction;

    class Shape;

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

    inline Float toRadians(const Float deg) noexcept { 
        using constants::Pi;
        return (Pi / 180.f) * deg; 
    }

    inline Float toDegrees(const Float rad) noexcept {
        using constants::Pi;
        return (180.f / Pi) * rad; 
    }

    template <typename T, typename U, typename V>
    inline T clamp(const T val, const U low, const V high) noexcept {
        static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && std::is_arithmetic_v<V>,
            "clamp can be used only with arithmetic types");

        if (val < low) {
            return low;
        }
        else if (val > high) {
            return high;
        }
        else {
            return val;
        }
    }

    template <typename T>
    inline auto lerp(const Float t, const T& a, const T& b) noexcept { 
        return (1.f - t) * a + t * b; 
    }

    inline constexpr Float gamma(int n) noexcept {
        using constants::MachineEpsilon;
        return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
    }
}
