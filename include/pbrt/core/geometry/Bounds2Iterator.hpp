#pragma once

#include <iterator>

#include "pbrt/core/math/Point2.hpp"

namespace idragnev::pbrt {
    template <typename T>
    class Bounds2Iterator
    {
        static_assert(std::is_integral_v<T>,
                      "Bounds2Iterator<T> requires T to be an integral type");

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = math::Point2<T>;

        Bounds2Iterator(const Bounds2<T>& b, const math::Point2<T>& p)
            : currentPoint(p)
            , bounds(&b) {}

        Bounds2Iterator& operator++() noexcept {
            ++currentPoint.x;
            if (currentPoint.x == bounds->max.x) {
                currentPoint.x = bounds->min.x;
                ++currentPoint.y;
            }
            return *this;
        }

        Bounds2Iterator operator++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Bounds2Iterator& rhs) const noexcept {
            return bounds == rhs.bounds && currentPoint == rhs.currentPoint;
        }

        bool operator!=(const Bounds2Iterator& rhs) const noexcept {
            return !(*this == rhs);
        }

        math::Point2<T> operator*() const { return currentPoint; }

    private:
        math::Point2<T> currentPoint;
        const Bounds2<T>* bounds;
    };
} // namespace idragnev::pbrt