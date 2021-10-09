#pragma once

#include "pbrt/core/sampling/Filter.hpp"

namespace idragnev::pbrt::filters {
    class TriangleFilter : public Filter
    {
    public:
        using Filter::Filter;

        Float eval(const Point2f& p) const override;
    };
} // namespace idragnev::pbrt::filters