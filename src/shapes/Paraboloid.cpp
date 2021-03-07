#include "pbrt/shapes/Paraboloid.hpp"
#include "pbrt/core/Bounds3.hpp"
#include "pbrt/core/EFloat.hpp"
#include "pbrt/core/Transformation.hpp"

namespace idragnev::pbrt::shapes {
    Paraboloid::Paraboloid(const Transformation& objectToWorld,
                           const Transformation& worldToObject,
                           const bool reverseOrientation,
                           const Float radius,
                           const Float zMin,
                           const Float zMax,
                           const Float phiMax) noexcept
        : Shape{objectToWorld, worldToObject, reverseOrientation}
        , radius(radius)
        , zMin(std::min(zMin, zMax))
        , zMax(std::max(zMin, zMax))
        , phiMax{math::toRadians(clamp(phiMax, 0.f, 360.f))} {}

    Bounds3f Paraboloid::objectBound() const {
        return Bounds3f{Point3f{-radius, -radius, zMin},
                        Point3f{radius, radius, zMax}};
    }

    Optional<HitRecord> Paraboloid::intersect(const Ray& rayInWorldSpace,
                                                   const bool) const {
        return intersectImpl<Optional<HitRecord>>(
            rayInWorldSpace,
            pbrt::nullopt,
            [this](const auto&... args) {
                return pbrt::make_optional(makeHitRecord(args...));
            });
    }

    bool Paraboloid::intersectP(const Ray& rayInWorldSpace, const bool) const {
        return intersectImpl<bool>(rayInWorldSpace, false, [](const auto&...) {
            return true;
        });
    }

    template <typename R, typename S, typename F>
    R Paraboloid::intersectImpl(const Ray& rayInWorldSpace,
                                F failure,
                                S success) const {
        const auto rayWithErrBound =
            worldToObjectTransform->transformWithErrBound(rayInWorldSpace);
        const auto& [ray, oErr, dErr] = rayWithErrBound;

        const auto intersectionParams = findIntersectionParams(ray, oErr, dErr);
        if (!intersectionParams.has_value()) {
            return failure;
        }

        const auto [t0, t1] = intersectionParams.value();
        if (t0.upperBound() > ray.tMax || t1.lowerBound() <= 0.f) {
            return failure;
        }

        auto tHit = t0;
        if (tHit.lowerBound() <= 0.f) {
            tHit = t1;
            if (tHit.upperBound() > ray.tMax) {
                return failure;
            }
        }

        auto hitPoint = ray(static_cast<Float>(tHit));
        auto phi = computePhi(hitPoint);
        if (hitPoint.z < zMin || hitPoint.z > zMax || phi > phiMax) {
            if (tHit == t1 || t1.upperBound() > ray.tMax) {
                return failure;
            }

            tHit = t1;
            hitPoint = ray(static_cast<Float>(tHit));
            phi = computePhi(hitPoint);
            if (hitPoint.z < zMin || hitPoint.z > zMax || phi > phiMax) {
                return failure;
            }
        }

        return success(rayWithErrBound, hitPoint, tHit, phi);
    }

    Optional<QuadraticRoots>
    Paraboloid::findIntersectionParams(const Ray& ray,
                                       const Vector3f& oErr,
                                       const Vector3f& dErr) const {
        const auto ox = EFloat(ray.o.x, oErr.x);
        const auto oy = EFloat(ray.o.y, oErr.y);
        const auto oz = EFloat(ray.o.z, oErr.z);
        const auto dx = EFloat(ray.d.x, dErr.x);
        const auto dy = EFloat(ray.d.y, dErr.y);
        const auto dz = EFloat(ray.d.z, dErr.z);

        const auto k = EFloat(zMax) / (EFloat(radius) * EFloat(radius));
        const auto a = k * (dx * dx + dy * dy);
        const auto b = 2 * k * (dx * ox + dy * oy) - dz;
        const auto c = k * (ox * ox + oy * oy) - oz;

        return solveQuadratic(a, b, c);
    }

    Float Paraboloid::computePhi(const Point3f& hitPoint) {
        const Float phi = std::atan2(hitPoint.y, hitPoint.x);
        return phi < 0.f ? (phi + 2 * math::constants::Pi) : phi;
    }

    HitRecord
    Paraboloid::makeHitRecord(const RayWithErrorBound& rayWithErrBound,
                              const Point3f& hitPoint,
                              const EFloat& t,
                              const Float phi) const {
        const auto dpdu =
            Vector3f(-phiMax * hitPoint.y, phiMax * hitPoint.x, 0.f);
        const auto dpdv =
            (zMax - zMin) * Vector3f(hitPoint.x / (2.f * hitPoint.z),
                                     hitPoint.y / (2.f * hitPoint.z),
                                     1.f);

        const auto d2Pduu =
            -phiMax * phiMax * Vector3f(hitPoint.x, hitPoint.y, 0.f);
        const auto d2Pduv = (zMax - zMin) * phiMax *
                            Vector3f(-hitPoint.y / (2.f * hitPoint.z),
                                     hitPoint.x / (2 * hitPoint.z),
                                     0.f);
        const auto d2Pdvv = -(zMax - zMin) * (zMax - zMin) *
                            Vector3f(hitPoint.x / (4 * hitPoint.z * hitPoint.z),
                                     hitPoint.y / (4 * hitPoint.z * hitPoint.z),
                                     0.f);
        const Float E = dot(dpdu, dpdu);
        const Float F = dot(dpdu, dpdv);
        const Float G = dot(dpdv, dpdv);
        const Vector3f N = normalize(cross(dpdu, dpdv));
        const Float e = dot(N, d2Pduu);
        const Float f = dot(N, d2Pduv);
        const Float g = dot(N, d2Pdvv);

        const Float invEGF2 = 1.f / (E * G - F * F);
        const auto dndu = Normal3f{(f * F - e * G) * invEGF2 * dpdu +
                                   (e * F - f * E) * invEGF2 * dpdv};
        const auto dndv = Normal3f{(g * F - f * G) * invEGF2 * dpdu +
                                   (f * F - g * E) * invEGF2 * dpdv};

        const auto pError = computeHitPointError(rayWithErrBound, t);

        const auto& ray = rayWithErrBound.ray;
        const auto wo = -ray.d;
        const Float u = phi / phiMax;
        const Float v = (hitPoint.z - zMin) / (zMax - zMin);

        const auto interaction = SurfaceInteraction{hitPoint,
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

    Vector3f
    Paraboloid::computeHitPointError(const RayWithErrorBound& rayWithErrBound,
                                     const EFloat& t) const {
        const auto& [ray, oErr, dErr] = rayWithErrBound;

        const auto ox = EFloat(ray.o.x, oErr.x);
        const auto oy = EFloat(ray.o.y, oErr.y);
        const auto oz = EFloat(ray.o.z, oErr.z);
        const auto dx = EFloat(ray.d.x, dErr.x);
        const auto dy = EFloat(ray.d.y, dErr.y);
        const auto dz = EFloat(ray.d.z, dErr.z);

        const EFloat px = ox + t * dx;
        const EFloat py = oy + t * dy;
        const EFloat pz = oz + t * dz;

        return Vector3f{px.absoluteError(),
                        py.absoluteError(),
                        pz.absoluteError()};
    }

    Float Paraboloid::area() const {
        const Float radius2 = radius * radius;
        const Float k = 4.f * zMax / radius2;

        return (radius2 * radius2 * phiMax / (12 * zMax * zMax)) *
               (std::pow(k * zMax + 1, 1.5f) - std::pow(k * zMin + 1, 1.5f));
    }
} // namespace idragnev::pbrt::shapes