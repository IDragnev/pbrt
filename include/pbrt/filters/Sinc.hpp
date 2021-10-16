#pragma once

#include "pbrt/core/sampling/Filter.hpp"

namespace idragnev::pbrt::filters {
    class LanczosSincFilter : public Filter
    {
    public:
        LanczosSincFilter(const Vector2f& radius, const Float tau)
            : Filter(radius)
            , tau(tau) {}

        Float eval(const Point2f& p) const override;

    private:
        Float sinc(Float x) const;
        Float windowedSinc(Float x, const Float radius) const;

    private:
        const Float tau = 3.f;
    };
} // namespace idragnev::pbrt::filters