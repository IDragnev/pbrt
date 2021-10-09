#include "pbrt/filters/Triangle.hpp"
#include "pbrt/core/math/Point2.hpp"

#include <algorithm>

namespace idragnev::pbrt::filters {
    Float TriangleFilter::eval(const Point2f& p) const {
        return std::max(Float(0), radius.x - std::abs(p.x)) *
               std::max(Float(0), radius.y - std::abs(p.y));
    }
} // namespace idragnev::pbrt::filters