#include "shapes/Sphere.hpp"
#include "core/Bounds3.hpp"
#include "core/Transformation.hpp"
#include "core/EFloat.hpp"

namespace idragnev::pbrt {
    Sphere::Sphere(const Transformation& objectToWorld, 
        const Transformation& worldToObject,
        const bool reverseOrientation,
        const Float radius,
        const Float zMin,
        const Float zMax,
        const Float phiMax) noexcept
        : Shape{ objectToWorld, worldToObject, reverseOrientation }
        , radius{ radius }
        , zMin{ clamp(std::min(zMin, zMax), -radius, radius) }
        , zMax{ clamp(std::max(zMin, zMax), -radius, radius) }
        , thetaMin{ 
            std::acos(
                clamp(std::min(zMin, zMax) / radius, -1.f, 1.f)
            ) 
          }
        , thetaMax{ 
            std::acos(
                clamp(std::max(zMin, zMax) / radius, -1.f, 1.f)
            ) 
          }
        , phiMax{ toRadians(clamp(phiMax, 0.f, 360.f)) }
    {
    }

    Bounds3f Sphere::objectBound() const {
        return Bounds3f{
            Point3f{ -radius, -radius, zMin },
            Point3f{  radius,  radius, zMax }
        };
    }

    std::optional<HitRecord> Sphere::intersect(const Ray& rayInWorldSpace, const bool) const {
        return intersectImpl<std::optional<HitRecord>>(
            rayInWorldSpace,
            std::nullopt,
            [this](const auto&... args) {
                return std::make_optional(makeHitRecord(args...));
            });
    }

    bool Sphere::intersectP(const Ray& rayInWorldSpace, const bool) const {
        return intersectImpl<bool>(
            rayInWorldSpace,
            false,
            [](const auto&...) {
                return true;
            });
    }

    //will be instantiated only in this translation unit so its ok to be defined here
    template <typename R, typename S, typename F>
    R Sphere::intersectImpl(const Ray& rayInWorldSpace, F failure, S success) const {
        const auto [ray, oErr, dErr] = worldToObjectTransform->transformWithErrBound(rayInWorldSpace);

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

        const auto liesInZClippedArea = [this](const Point3f& hitPoint) {
            return (zMin > -radius && hitPoint.z < zMin) ||
                   (zMax <  radius && hitPoint.z > zMax);
        };

        auto hitPoint = computeHitPoint(ray, tShapeHit);
        auto phi = computePhi(hitPoint);
        if (liesInZClippedArea(hitPoint) || phi > phiMax) {
            if (tShapeHit == t1 || t1.upperBound() > ray.tMax) {
                return failure;
            }

            tShapeHit = t1;
            hitPoint = computeHitPoint(ray, tShapeHit);
            phi = computePhi(hitPoint);
            if (liesInZClippedArea(hitPoint) || phi > phiMax) {
                return failure;
            }
        }

        return success(ray, hitPoint, tShapeHit, phi);
    }

    std::optional<QuadraticRoots> Sphere::findIntersectionParams(const Ray& ray, const Vector3f& oErr, const Vector3f& dErr) const {
        const auto ox = EFloat(ray.o.x, oErr.x);
        const auto oy = EFloat(ray.o.y, oErr.y);
        const auto oz = EFloat(ray.o.z, oErr.z);
        const auto dx = EFloat(ray.d.x, dErr.x);
        const auto dy = EFloat(ray.d.y, dErr.y);
        const auto dz = EFloat(ray.d.z, dErr.z);

        const auto a = dx * dx + dy * dy + dz * dz;
        const auto b = 2.f * (dx * ox + dy * oy + dz * oz);
        const auto c = ox * ox + oy * oy + oz * oz - EFloat(radius) * EFloat(radius);

        return solveQuadratic(a, b, c);
    }

    Float Sphere::computePhi(const Point3f& hitPoint) {
        const Float phi = std::atan2(hitPoint.y, hitPoint.x);
        return phi < 0.f ? (phi + 2 * constants::Pi) : phi;
    }

    Point3f Sphere::computeHitPoint(const Ray& ray, const EFloat& t) const {
        auto p = ray(static_cast<Float>(t));
        p *= radius / distance(p, Point3f(0.f, 0.f, 0.f));
        if (p.x == 0.f && p.y == 0.f) {
            p.x = 1e-5f * radius;
        }

        return p;
    }

    HitRecord Sphere::makeHitRecord(const Ray& ray, const Point3f& hitPoint, const EFloat& tShapeHit, const Float phi) const {
        const Float theta = std::acos(clamp(hitPoint.z / radius, -1.f, 1.f));
        const Float u = phi / phiMax;
        const Float v = (theta - thetaMin) / (thetaMax - thetaMin);

        const Float zRadius = std::sqrt(hitPoint.x * hitPoint.x + hitPoint.y * hitPoint.y);
        const Float invZRadius = 1.f / zRadius;
        const Float cosPhi = hitPoint.x * invZRadius;
        const Float sinPhi = hitPoint.y * invZRadius;
        const Vector3f dpdu(-phiMax * hitPoint.y, phiMax * hitPoint.x, 0.f);
        const Vector3f dpdv = (thetaMax - thetaMin) * Vector3f(hitPoint.z * cosPhi, hitPoint.z * sinPhi, -radius * std::sin(theta));

        const Vector3f d2Pduu = -phiMax * phiMax * Vector3f(hitPoint.x, hitPoint.y, 0);
        const Vector3f d2Pduv = (thetaMax - thetaMin) * hitPoint.z * phiMax * Vector3f(-sinPhi, cosPhi, 0.f);
        const Vector3f d2Pdvv = -(thetaMax - thetaMin) * (thetaMax - thetaMin) * Vector3f(hitPoint.x, hitPoint.y, hitPoint.z);

        const Float E = dot(dpdu, dpdu);
        const Float F = dot(dpdu, dpdv);
        const Float G = dot(dpdv, dpdv);
        const Vector3f N = normalize(cross(dpdu, dpdv));
        const Float e = dot(N, d2Pduu);
        const Float f = dot(N, d2Pduv);
        const Float g = dot(N, d2Pdvv);

        const Float invEGF2 = 1 / (E * G - F * F);
        const Normal3f dndu = Normal3f((f * F - e * G) * invEGF2 * dpdu + (e * F - f * E) * invEGF2 * dpdv);
        const Normal3f dndv = Normal3f((g * F - f * G) * invEGF2 * dpdu + (f * F - g * E) * invEGF2 * dpdv);

        const Vector3f pError = gamma(5) * abs(Vector3f{ hitPoint });

        const auto wo = -ray.d;

        const auto interaction = SurfaceInteraction{
            hitPoint,
            pError,
            Point2f{u, v},
            wo,
            dpdu, dpdv,
            dndu, dndv,
            ray.time,
            this
        };
        
        auto result = HitRecord{};
        result.t = static_cast<Float>(tShapeHit);
        result.interaction = (*objectToWorldTransform)(interaction);
        return result;
    }

    Float Sphere::area() const {
        return phiMax * radius * (zMax - zMin);
    }
} //namespace idragnev::pbrt