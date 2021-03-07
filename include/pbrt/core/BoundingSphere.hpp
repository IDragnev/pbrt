#pragma once

#include "core.hpp"
#include "math/Fwd.hpp"

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
    using BoundingSphere2 = detail::BoundingSphere<math::Point2<T>>;
    template <typename T>
    using BoundingSphere3 = detail::BoundingSphere<math::Point3<T>>;
} // namespace idragnev::pbrt