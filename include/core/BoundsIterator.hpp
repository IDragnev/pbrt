#pragma once

#include <iterator>

namespace idragnev::pbrt {
    template <typename BoundsT>
    class BoundsIterator
    {
        using PointType = typename BoundsT::PointType;

        static_assert(std::is_integral_v<typename PointType::UnderlyingType>,
            "Cannot iterate over this bounds type - its PointType has non-integral underlying type");

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = PointType;

        BoundsIterator(const BoundsT& b, const PointType& p)
            : currentPoint(p)
            , bounds(&b) 
        {
        }

        BoundsIterator& operator++() noexcept {
            ++currentPoint.x;
            if (currentPoint.x == bounds->max.x) {
                currentPoint.x = bounds->min.x;
                ++currentPoint.y;
            }
            return *this;
        }

        BoundsIterator operator++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const BoundsIterator& rhs) const noexcept {
            return bounds == rhs.bounds && currentPoint == rhs.currentPoint;
        }

        bool operator!=(const BoundsIterator& rhs) const noexcept {
            return !(*this == rhs);
        }

        PointType operator*() const { 
            return currentPoint;
        }

    private:
        PointType currentPoint;
        const BoundsT* bounds;
    };
} //namespace idragnev::pbrt