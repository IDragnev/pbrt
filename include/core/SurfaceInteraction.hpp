#pragma once

#include "core.hpp"
#include "Interaction.hpp"
#include "Point2.hpp"
#include "Vector3.hpp"
#include "Normal3.hpp"

namespace idragnev::pbrt {
    class SurfaceInteraction : public Interaction
    {
    public:
        SurfaceInteraction() = default;
        SurfaceInteraction(const Point3f& p,
                           const Vector3f& pError,
                           const Point2f& uv,
                           const Vector3f& wo,
                           const Vector3f& dpdu,
                           const Vector3f& dpdv,
                           const Normal3f& dndu,
                           const Normal3f& dndv,
                           const Float time,
                           const Shape* sh,
                           const std::size_t faceIndex = 0);

        void setShadingGeometry(const Vector3f& dpdus,
                                const Vector3f& dpdvs,
                                const Normal3f& dndus,
                                const Normal3f& dndvs,
                                const bool isOrientationAuthoritative);

    public:
        Point2f uv;
        Vector3f dpdu;
        Vector3f dpdv;
        Normal3f dndu;
        Normal3f dndv;
        const Shape* shape = nullptr;
        std::size_t faceIndex = 0;
        struct
        {
            Normal3f n;
            Vector3f dpdu, dpdv;
            Normal3f dndu, dndv;
        } shading;
    };
} // namespace idragnev::pbrt