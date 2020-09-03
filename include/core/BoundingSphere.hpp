#pragma once

#include "core/core.hpp"

namespace idragnev::pbrt {
    namespace detail {
        template <typename PointT>
        struct BoundingSphere
        {
            PointT center;
            Float radius = 0.0;
        };
    } // namespace detail

    template <typename T>
    using BoundingSphere2 = detail::BoundingSphere<Point2<T>>;
    template <typename T>
    using BoundingSphere3 = detail::BoundingSphere<Point3<T>>;
} // namespace idragnev::pbrt