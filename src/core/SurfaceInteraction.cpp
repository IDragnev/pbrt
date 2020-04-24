#include "SurfaceInteraction.hpp"
#include "Shape.hpp"

namespace idragnev::pbrt {
    SurfaceInteraction::SurfaceInteraction(const Point3f& p, const Vector3f& pError, 
        const Point2f& uv, const Vector3f& wo, 
        const Vector3f& dpdu, const Vector3f& dpdv, 
        const Normal3f& dndu, const Normal3f& dndv, 
        const Float time, const Shape* sh, 
        const std::size_t faceIndex)
        : Interaction{p, Normal3f{ normalize(cross(dpdu, dpdv)) }, pError, wo, time}
        , uv{uv}
        , dpdu{dpdu}
        , dpdv{dpdv}
        , dndu{dndu}
        , dndv{dndv}
        , shape{shape}
        , faceIndex(faceIndex)
    {
        shading.n = n;
        shading.dpdu = dpdu;
        shading.dpdv = dpdv;
        shading.dndu = dndu;
        shading.dndv = dndv;

        if (shape && (shape->reverseOrientation ^ shape->transformSwapsHandedness)) {
            n *= -1;
            shading.n *= -1;
        }
    }

    void SurfaceInteraction::setShadingGeometry(
        const Vector3f& dpdus,
        const Vector3f& dpdvs,
        const Normal3f& dndus,
        const Normal3f& dndvs,
        const bool isOrientationAuthoritative)
    {
        shading.n = [&] {
            const auto n = Normal3f{ normalize(cross(dpdus, dpdvs)) };
            return (shape && (shape->reverseOrientation ^ shape->transformSwapsHandedness)) 
                    ? -n 
                    : n;
        }();

        if (isOrientationAuthoritative) {
            n = faceforward(n, shading.n);
        }
        else {
            shading.n = faceforward(shading.n, n);
        }

        shading.dpdu = dpdus;
        shading.dpdv = dpdvs;
        shading.dndu = dndus;
        shading.dndv = dndvs;
    }
} //namespace idragnev::pbrt