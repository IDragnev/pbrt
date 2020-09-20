#include "shapes/Cylinder.hpp"
#include "core/Bounds3.hpp"
#include "core/Transformation.hpp"
#include "core/EFloat.hpp"

namespace idragnev::pbrt {
    Cylinder::Cylinder(const Transformation& objectToWorld,
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
        , phiMax(toRadians(clamp(phiMax, 0.f, 360.f))) {}

    Bounds3f Cylinder::objectBound() const {
        return Bounds3f{Point3f(-radius, -radius, zMin),
                        Point3f(radius, radius, zMax)};
    }

    std::optional<HitRecord> Cylinder::intersect(const Ray& rayInWorldSpace,
                                                 const bool) const {
        return intersectImpl<std::optional<HitRecord>>(
            rayInWorldSpace,
            std::nullopt,
            [this](const auto&... args) {
                return std::make_optional(makeHitRecord(args...));
            });
    }

    bool Cylinder::intersectP(const Ray& rayInWorldSpace, const bool) const {
        return intersectImpl<bool>(rayInWorldSpace, false, [](const auto&...) {
            return true;
        });
    }

    // will be instantiated only in this translation unit
    // so it is fine to define it here
    template <typename R, typename S, typename F>
    R Cylinder::intersectImpl(const Ray& rayInWorldSpace,
                              F failure,
                              S success) const {
        const auto [ray, oErr, dErr] =
            worldToObjectTransform->transformWithErrBound(rayInWorldSpace);

        const auto intersectionParams = findIntersectionParams(ray, oErr, dErr);
        if (!intersectionParams.has_value()) {
            return failure;
        }

        const auto [t0, t1] = intersectionParams.value();
        if (t0.upperBound() > ray.tMax || t1.lowerBound() <= 0.f) {
            return failure;
        }

        auto tShapeHit = t0;
        if (tShapeHit.lowerBound() <= 0.f) {
            tShapeHit = t1;
            if (tShapeHit.upperBound() > ray.tMax) {
                return failure;
            }
        }

        auto hitPoint = computeHitPoint(ray, tShapeHit);
        auto phi = computePhi(hitPoint);
        if (hitPoint.z < zMin || hitPoint.z > zMax || phi > phiMax) {
            if (tShapeHit == t1 || t1.upperBound() > ray.tMax) {
                return failure;
            }

            tShapeHit = t1;

            hitPoint = computeHitPoint(ray, tShapeHit);
            phi = computePhi(hitPoint);
            if (hitPoint.z < zMin || hitPoint.z > zMax || phi > phiMax) {
                return failure;
            }
        }

        return success(ray, hitPoint, tShapeHit, phi);
    }

    std::optional<QuadraticRoots>
    Cylinder::findIntersectionParams(const Ray& ray,
                                     const Vector3f& oErr,
                                     const Vector3f& dErr) const {
        const auto ox = EFloat(ray.o.x, oErr.x);
        const auto oy = EFloat(ray.o.y, oErr.y);
        const auto dx = EFloat(ray.d.x, dErr.x);
        const auto dy = EFloat(ray.d.y, dErr.y);

        const auto a = dx * dx + dy * dy;
        const auto b = 2 * (dx * ox + dy * oy);
        const auto c = ox * ox + oy * oy - EFloat(radius) * EFloat(radius);

        return solveQuadratic(a, b, c);
    }

    Point3f Cylinder::computeHitPoint(const Ray& ray, const EFloat& t) const {
        const auto p = ray(static_cast<Float>(t));
        const Float hitRad = std::sqrt(p.x * p.x + p.y * p.y);
        const Float k = radius / hitRad;

        return Point3f{p.x * k, p.y * k, p.z};
    }

    Float Cylinder::computePhi(const Point3f& hitPoint) {
        const Float phi = std::atan2(hitPoint.y, hitPoint.x);
        return phi < 0.f ? (phi + 2 * constants::Pi) : phi;
    }

    HitRecord Cylinder::makeHitRecord(const Ray& ray,
                                      const Point3f& hitPoint,
                                      const EFloat& t,
                                      const Float phi) const {
        const Float u = phi / phiMax;
        const Float v = (hitPoint.z - zMin) / (zMax - zMin);

        const auto dpdu =
            Vector3f{-phiMax * hitPoint.y, phiMax * hitPoint.x, 0.f};
        const auto dpdv = Vector3f{0.f, 0.f, zMax - zMin};

        const auto d2Pduu =
            -phiMax * phiMax * Vector3f{hitPoint.x, hitPoint.y, 0.f};
        const auto d2Pduv = Vector3f{0.f, 0.f, 0.f};
        const auto d2Pdvv = Vector3f{0.f, 0.f, 0.f};

        const Float E = dot(dpdu, dpdu);
        const Float F = dot(dpdu, dpdv);
        const Float G = dot(dpdv, dpdv);
        const Vector3f N = normalize(cross(dpdu, dpdv));
        const Float e = dot(N, d2Pduu);
        const Float f = dot(N, d2Pduv);
        const Float g = dot(N, d2Pdvv);

        const Float invEGF2 = 1 / (E * G - F * F);
        const auto dndu = Normal3f{(f * F - e * G) * invEGF2 * dpdu +
                                   (e * F - f * E) * invEGF2 * dpdv};
        const auto dndv = Normal3f{(g * F - f * G) * invEGF2 * dpdu +
                                   (f * F - g * E) * invEGF2 * dpdv};

        const Vector3f pError =
            gamma(3) * abs(Vector3f(hitPoint.x, hitPoint.y, 0.f));
        const auto wo = -ray.d;

        const auto interaction = SurfaceInteraction{hitPoint,
                                                    pError,
                                                    Point2f(u, v),
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

    Float Cylinder::area() const { return (zMax - zMin) * radius * phiMax; }
} // namespace idragnev::pbrt