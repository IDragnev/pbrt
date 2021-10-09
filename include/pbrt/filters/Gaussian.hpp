#pragma once

#include "pbrt/core/sampling/Filter.hpp"

namespace idragnev::pbrt::filters {
    class GaussianFilter : public Filter
    {
    public:
        GaussianFilter(const Vector2f& radius, const Float alpha);

        Float eval(const Point2f& p) const override;

    private:
        Float gaussian1d(const Float d, const Float expv) const;

    private:
        Float alpha = 0.f;
        Float expX = 0.f;
        Float expY = 0.f;
    };
} // namespace idragnev::pbrt::filters