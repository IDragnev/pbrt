#include "pbrt/filters/Gaussian.hpp"
#include "pbrt/core/math/Point2.hpp"

namespace idragnev::pbrt::filters {
    GaussianFilter::GaussianFilter(const Vector2f& radius, const Float alpha)
        : Filter(radius)
        , alpha(alpha)
        , expX(std::exp(-alpha * radius.x * radius.x))
        , expY(std::exp(-alpha * radius.y * radius.y)) {}

    Float GaussianFilter::eval(const Point2f& p) const {
        return gaussian1d(p.x, expX) * gaussian1d(p.y, expY);
    }

    Float GaussianFilter::gaussian1d(const Float d, const Float expv) const {
        return std::max(Float(0), Float(std::exp(-alpha * d * d) - expv));
    }
} // namespace idragnev::pbrt::filters