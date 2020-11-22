#include "core/Ray.hpp"

namespace idragnev::pbrt {
    Ray::Ray(const Point3f& o,
             const Vector3f& d,
             const Float tMax,
             const Float time,
             const Medium* medium)
        : o(o)
        , d(d)
        , tMax(tMax)
        , time(time)
        , medium(medium) {}
} // namespace idragnev::pbrt