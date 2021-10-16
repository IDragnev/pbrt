#pragma once

#include "pbrt/core/sampling/Filter.hpp"

namespace idragnev::pbrt::filters {
    class MitchellFilter : public Filter
    {
    public:
        MitchellFilter(const Vector2f& radius, const Float B, const Float C)
            : Filter(radius)
            , B(B)
            , C(C) {}

        // no radius checks are performed
        Float eval(const Point2f& p) const override;

    private:
        Float mitchell1d(Float x) const;

    private:
        const Float B = 0.f;
        const Float C = 0.f;
    };
} // namespace idragnev::pbrt::filters