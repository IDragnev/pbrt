#pragma once

#include "math/Fwd.hpp"

#include <limits>
#include <cmath>
#include <cstring>
#include <cstdint>

namespace idragnev::pbrt {

#ifdef PBRT_FLOAT_AS_DOUBLE
    using Float = double;
#else
    using Float = float;
#endif

    namespace constants {
        // clang-format off
        inline constexpr Float MaxFloat = std::numeric_limits<Float>::max();
        inline constexpr Float Infinity = std::numeric_limits<Float>::infinity();
        inline constexpr Float MachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5f;
        inline constexpr Float ShadowEpsilon = 0.0001f;
        // clang-format on
    } // namespace constants

    using Basis3f = math::Basis3<Float>;

    using Vector2f = math::Vector2<Float>;
    using Vector2i = math::Vector2<int>;
    using Vector3f = math::Vector3<Float>;
    using Vector3i = math::Vector3<int>;

    using Point2f = math::Point2<Float>;
    using Point2i = math::Point2<int>;
    using Point3f = math::Point3<Float>;
    using Point3i = math::Point3<int>;

    using Normal3f = math::Normal3<Float>;

    using Intervalf = math::Interval<Float>;

    template <typename T>
    class Bounds2;
    template <typename T>
    class Bounds3;

    using Bounds2f = Bounds2<Float>;
    using Bounds2i = Bounds2<int>;
    using Bounds3f = Bounds3<Float>;

    class Ray;
    class RayDifferential;

    struct RayWithErrorBound;
    class Transformation;

    class Quaternion;
    struct TRS;
    class AnimatedTransformation;

    class Interaction;
    class SurfaceInteraction;

    class Shape;
    class Primitive;
    class GeometricPrimitive;
    class TransformedPrimitive;
    class Aggregate;

    class Light;
    class AreaLight;

    enum class TransportMode;
    class Material;

    class BSDF;
    class BSSRDF;

    class Medium;
    struct MediumInterface;

    class EFloat;
    struct QuadraticRoots;

    template <typename T>
    class Texture;

    class Film;

    template <typename T,
              typename = std::enable_if_t<std::is_floating_point_v<T>>>
    inline bool isNaN(const T x) {
        return std::isnan(x);
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    inline bool isNaN(const T&) {
        return false;
    }

    template <typename T, typename U, typename V>
    inline T clamp(const T val, const U low, const V high) noexcept {
        static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<U> &&
                          std::is_arithmetic_v<V>,
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
    inline constexpr auto lerp(const Float t, const T& a, const T& b) noexcept {
        return (1.f - t) * a + t * b;
    }

    inline constexpr Float gamma(const int n) noexcept {
        using constants::MachineEpsilon;
        return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
    }

    namespace detail {
        template <typename R, typename T>
        inline R floatToBits(const T f) noexcept {
            R result;
            std::memcpy(&result, &f, sizeof(T));
            return result;
        }

        template <typename R, typename T>
        inline R bitsToFloat(const T bits) noexcept {
            R f;
            std::memcpy(&f, &bits, sizeof(T));
            return f;
        }
    } // namespace detail

    inline std::uint32_t floatToBits(const float f) noexcept {
        return detail::floatToBits<std::uint32_t>(f);
    }

    inline std::uint64_t floatToBits(const double f) noexcept {
        return detail::floatToBits<std::uint64_t>(f);
    }

    inline float bitsToFloat(const std::uint32_t bits) noexcept {
        return detail::bitsToFloat<float>(bits);
    }

    inline double bitsToFloat(const std::uint64_t bits) noexcept {
        return detail::bitsToFloat<double>(bits);
    }

    float nextFloatUp(float v) noexcept;
    float nextFloatDown(float v) noexcept;

    double nextFloatUp(double v, const int delta = 1) noexcept;
    double nextFloatDown(double v, const int delta = 1) noexcept;
} // namespace idragnev::pbrt
