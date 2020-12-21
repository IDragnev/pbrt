#include "pbrt/shapes/Disk.hpp"
#include "pbrt/core/Transformation.hpp"
#include "pbrt/core/EFloat.hpp"
#include "pbrt/core/Bounds3.hpp"

namespace idragnev::pbrt::shapes {
    Disk::Disk(const Transformation& objectToWorld,
               const Transformation& worldToObject,
               const bool reverseOrientation,
               const Float height,
               const Float radius,
               const Float innerRadius,
               const Float phiMax) noexcept
        : Shape{objectToWorld, worldToObject, reverseOrientation}
        , height{height}
        , radius{radius}
        , innerRadius{innerRadius}
        , phiMax{toRadians(clamp(phiMax, 0.f, 360.f))} {}

    Bounds3f Disk::objectBound() const {
        return Bounds3f{Point3f{-radius, -radius, height},
                        Point3f{radius, radius, height}};
    }

    Optional<HitRecord> Disk::intersect(const Ray& ray, const bool) const {
        return intersectImpl<Optional<HitRecord>>(
            ray,
            pbrt::nullopt,
            [this](const auto&... args) {
                return pbrt::make_optional(makeHitRecord(args...));
            });
    }

    bool Disk::intersectP(const Ray& ray, const bool) const {
        return intersectImpl<bool>(ray, false, [](const auto&...) {
            return true;
        });
    }

    // will be instantiated only in this translation unit
    // so it is fine to define it here
    template <typename R, typename S, typename F>
    R Disk::intersectImpl(const Ray& rayInWorldSpace,
                          F failure,
                          S success) const {
        const auto [ray, oErr, dErr] =
            worldToObjectTransform->transformWithErrBound(rayInWorldSpace);

        if (ray.d.z == 0.f) {
            return failure;
        }

        const Float tShapeHit = (height - ray.o.z) / ray.d.z;
        if (tShapeHit <= 0.f || tShapeHit >= ray.tMax) {
            return failure;
        }

        const auto hitPoint = ray(tShapeHit);
        const Float distToCenterSquared =
            hitPoint.x * hitPoint.x + hitPoint.y * hitPoint.y;
        if (distToCenterSquared > radius * radius ||
            distToCenterSquared < innerRadius * innerRadius)
        {
            return failure;
        }

        const auto phi = computePhi(hitPoint);
        if (phi > phiMax) {
            return failure;
        }

        return success(ray, hitPoint, tShapeHit, phi, distToCenterSquared);
    }

    Float Disk::computePhi(const Point3f& hitPoint) {
        const Float phi = std::atan2(hitPoint.y, hitPoint.x);
        return phi < 0.f ? (phi + 2 * constants::Pi) : phi;
    }

    HitRecord Disk::makeHitRecord(const Ray& ray,
                                  const Point3f& hitPoint,
                                  const EFloat& t,
                                  const Float phi,
                                  const Float distToCenterSquared) const {
        const Float u = phi / phiMax;
        const Float rHit = std::sqrt(distToCenterSquared);
        const Float v = (radius - rHit) / (radius - innerRadius);
        const auto dpdu =
            Vector3f(-phiMax * hitPoint.y, phiMax * hitPoint.x, 0.f);
        const auto dpdv = ((innerRadius - radius) / rHit) *
                          Vector3f(hitPoint.x, hitPoint.y, 0.f);
        const auto dndu = Normal3f{0.f, 0.f, 0.f};
        const auto dndv = Normal3f{0.f, 0.f, 0.f};

        const auto p = Point3f{hitPoint.x, hitPoint.y, height};
        const auto pError = Vector3f{0.f, 0.f, 0.f};
        const auto wo = -ray.d;

        const auto interaction = SurfaceInteraction{p,
                                                    pError,
                                                    Point2f{u, v},
                                                    wo,
                                                    dpdu,
                                                    dpdv,
                                                    dndu,
                                                    dndv,
                                                    ray.time,
                                                    this};

        HitRecord result;
        result.interaction = (*objectToWorldTransform)(interaction);
        result.t = static_cast<Float>(t);

        return result;
    }

    Float Disk::area() const {
        return phiMax * 0.5f * (radius * radius - innerRadius * innerRadius);
    }
} // namespace idragnev::pbrt::shapes