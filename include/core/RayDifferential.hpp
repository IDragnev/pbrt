#pragma once

#include "core/Ray.hpp"

namespace idragnev::pbrt {
    class RayDifferential : public Ray
    {
    public:
        using Ray::Ray;
        RayDifferential() = default;
        RayDifferential(const Ray& ray) : Ray(ray) {}

        bool hasNaNs() const;

        void scaleDifferentials(Float s);

    public:
        bool hasDifferentials = false;
        Point3f rxOrigin;
        Point3f ryOrigin;
        Vector3f rxDirection;
        Vector3f ryDirection;
    };
} // namespace idragnev::pbrt