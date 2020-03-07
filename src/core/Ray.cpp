#include "Ray.hpp"

namespace idragnev::pbrt {
    Ray::Ray(const Point3f& o, const Vector3f& d, Float tMax, Float time)
        : o(o)
        , d(d)
        , tMax(tMax)
        , time(time)
    {
    }
} //namespace idragnev::pbrt