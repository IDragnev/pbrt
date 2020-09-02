#include "core/Ray.hpp"

namespace idragnev::pbrt {
    Ray::Ray(const Point3f& o, const Vector3f& d, const Float tMax, const Float time)
        : o(o)
        , d(d)
        , tMax(tMax)
        , time(time)
    {
    }
} //namespace idragnev::pbrt