#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/math/Vector2.hpp"

namespace idragnev::pbrt {
    class Filter
    {
    public:
        Filter(const Vector2f& radius)
            : radius(radius)
            , invRadius(Vector2f(1.f / radius.x, 1.f / radius.y)) {}
        virtual ~Filter() = default;

        virtual Float eval(const Point2f& p) const = 0;

        const Vector2f radius;
        const Vector2f invRadius;
    };
} // namespace idragnev::pbrt