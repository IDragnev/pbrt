#pragma once

namespace idragnev::pbrt {
    template <typename T>
    Bounds2<T>::Bounds2() {
        constexpr auto minNum = std::numeric_limits<T>::lowest();
        constexpr auto maxNum = std::numeric_limits<T>::max();
        min = {maxNum, maxNum};
        max = {minNum, minNum};
    }

    template <typename T>
    inline Bounds2<T>::Bounds2(const math::Point2<T>& p) : min(p)
                                                         , max(p) {}

    template <typename T>
    Bounds2<T>::Bounds2(const math::Point2<T>& p1, const math::Point2<T>& p2)
        : min{math::min(p1, p2)}
        , max{math::max(p1, p2)} {}

    template <typename T>
    template <typename U>
    inline Bounds2<T>::operator Bounds2<U>() const {
        static_assert(std::is_convertible_v<T, U>,
                      "Cannot convert the underlying type");
        return Bounds2<U>{math::Point2<U>{min}, math::Point2<U>{max}};
    }

    template <typename T>
    inline math::Vector2<T> Bounds2<T>::diagonal() const {
        return max - min;
    }

    template <typename T>
    inline T Bounds2<T>::area() const {
        const auto d = diagonal();
        return d.x * d.y;
    }

    template <typename T>
    inline std::size_t Bounds2<T>::maximumExtent() const {
        const auto diag = diagonal();
        return (diag.x > diag.y) ? 0 : 1;
    }

    template <typename T>
    inline math::Point2<T>& Bounds2<T>::operator[](std::size_t i) {
        return const_cast<math::Point2<T>&>(
            static_cast<const Bounds2&>(*this)[i]);
    }

    template <typename T>
    inline const math::Point2<T>& Bounds2<T>::operator[](std::size_t i) const {
        assert(i < 2);
        return (i == 0) ? min : max;
    }

    template <typename T>
    math::Vector2<T> Bounds2<T>::offset(const math::Point2<T>& p) const {
        auto o = p - min;
        if (max.x > min.x) {
            o.x /= max.x - min.x;
        }
        if (max.y > min.y) {
            o.y /= max.y - min.y;
        }
        return o;
    }

    template <typename T>
    BoundingSphere2<T> Bounds2<T>::boundingSphere() const {
        const auto center = (min + max) / 2;
        BoundingSphere2<T> result;
        result.center = center;
        result.radius = inside(center, *this) ? distance(center, max) : 0.0;
        return result;
    }

    template <typename T>
    math::Point2<T> lerp(const Bounds2<T>& bounds, const Point2f& t) {
        return {pbrt::lerp(t.x, bounds.min.x, bounds.max.x),
                pbrt::lerp(t.y, bounds.min.y, bounds.max.y)};
    }

    template <typename T>
    Bounds2<T> unionOf(const Bounds2<T>& b, const math::Point2<T>& p) {
        Bounds2<T> result;
        result.min = min(b.min, p);
        result.max = max(b.max, p);
        return result;
    }

    template <typename T>
    Bounds2<T> unionOf(const Bounds2<T>& a, const Bounds2<T>& b) {
        Bounds2<T> result;
        result.min = min(a.min, b.min);
        result.max = max(a.max, b.max);
        return result;
    }

    template <typename T>
    Bounds2<T> intersectionOf(const Bounds2<T>& b1, const Bounds2<T>& b2) {
        // Important: assign to min/max directly and don't call the Bounds2()
        // constructor, since it takes min/max of the points passed to it.  In
        // turn, that breaks returning an invalid bound for the case where we
        // intersect non-overlapping bounds (as we'd like to happen).
        Bounds2<T> result;
        result.min = max(b1.min, b2.min);
        result.max = min(b1.max, b2.max);
        return result;
    }

    template <typename T>
    bool overlap(const Bounds2<T>& left, const Bounds2<T>& right) noexcept {
        const auto overlapX =
            (left.max.x >= right.min.x) && (left.min.x <= right.max.x);
        const auto overlapY =
            (left.max.y >= right.min.y) && (left.min.y <= right.max.y);
        return overlapX && overlapY;
    }

    template <typename T>
    bool inside(const math::Point2<T>& p, const Bounds2<T>& bounds) noexcept {
        return p.x >= bounds.min.x && p.x <= bounds.max.x &&
               p.y >= bounds.min.y && p.y <= bounds.max.y;
    }

    template <typename T>
    bool insideExclusive(const math::Point2<T>& p,
                         const Bounds2<T>& bounds) noexcept {
        return p.x >= bounds.min.x && p.x < bounds.max.x &&
               p.y >= bounds.min.y && p.y < bounds.max.y;
    }

    template <typename T, typename U>
    Bounds2<T> expand(const Bounds2<T>& bounds, U delta) {
        static_assert(std::is_arithmetic_v<U>,
                      "Cannot expand with non-arithmetic type");
        const auto v = math::Vector2<T>{delta, delta};
        return Bounds2<T>{bounds.min - v, bounds.max + v};
    }

    template <typename T>
    inline bool operator==(const Bounds2<T>& a, const Bounds2<T>& b) {
        return a.min == b.min && a.max == b.max;
    }

    template <typename T>
    inline bool operator!=(const Bounds2<T>& a, const Bounds2<T>& b) {
        return !(a == b);
    }

    template <typename T, typename>
    inline Bounds2Iterator<T> begin(const Bounds2<T>& bounds) {
        return {bounds, bounds.min};
    }

    template <typename T, typename>
    Bounds2Iterator<T> end(const Bounds2<T>& bounds) {
        using Point = typename Bounds2<T>::PointType;
        const auto isDegenerate =
            bounds.min.x >= bounds.max.x || bounds.min.y >= bounds.max.y;
        const auto end =
            !isDegenerate ? Point{bounds.min.x, bounds.max.y} : bounds.min;
        return {bounds, end};
    }
} // namespace idragnev::pbrt