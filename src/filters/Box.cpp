#include "pbrt/filters/Box.hpp"

namespace idragnev::pbrt::filters {
    // assumes that callers always pass points within the filter radius
    Float BoxFilter::eval(const Point2f&) const { return 1.f; }
} // namespace idragnev::pbrt::filters