#include "shapes/Cone.hpp"
#include "core/Bounds3.hpp"
#include "core/Transformation.hpp"
#include "core/EFloat.hpp"

namespace idragnev::pbrt {
    Cone::Cone(const Transformation& objectToWorld,
               const Transformation& worldToObject,
               const bool reverseOrientation,
               const Float height,
               const Float radius,
               const Float phiMax) noexcept
        : Shape{objectToWorld, worldToObject, reverseOrientation}
        , radius(radius)
        , height(height)
        , phiMax{toRadians(clamp(phiMax, 0.f, 360.f))} {}

    Bounds3f Cone::objectBound() const {
        return Bounds3f{Point3f{-radius, -radius, 0.f},
                        Point3f{radius, radius, height}};
    }

    std::optional<HitRecord> Cone::intersect(const Ray& ray, const bool) const {
        return intersectImpl<std::optional<HitRecord>>(
            ray,
            std::nullopt,
            [this](const auto&... args) {
                return std::make_optional(makeHitRecord(args...));
            });
    }

    bool Cone::intersectP(const Ray& ray, const bool) const {
        return intersectImpl<bool>(ray, false, [](const auto&...) {
            return true;
        });
    }

    // will be instantiated only in this translation unit so it's ok to be
    // defined here
    template <typename R, typename S, typename F>
    R Cone::intersectImpl(const Ray& rayInWorldSpace,
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
        if (hitPoint.z < 0.f || hitPoint.z > height || phi > phiMax) {
            if (tHit == t1 || t1.upperBound() > ray.tMax) {
                return failure;
            }

            tHit = t1;
            hitPoint = ray(static_cast<Float>(tHit));
            phi = computePhi(hitPoint);
            if (hitPoint.z < 0.f || hitPoint.z > height || phi > phiMax) {
                return failure;
            }
        }

        return success(rayWithErrBound, hitPoint, tHit, phi);
    }

    std::optional<QuadraticRoots>
    Cone::findIntersectionParams(const Ray& ray,
                                 const Vector3f& oErr,
                                 const Vector3f& dErr) const {
        const auto ox = EFloat(ray.o.x, oErr.x);
        const auto oy = EFloat(ray.o.y, oErr.y);
        const auto oz = EFloat(ray.o.z, oErr.z);
        const auto dx = EFloat(ray.d.x, dErr.x);
        const auto dy = EFloat(ray.d.y, dErr.y);
        const auto dz = EFloat(ray.d.z, dErr.z);

        const auto k = [this] {
            const auto x = EFloat(radius) / EFloat(height);
            return x * x;
        }();

        const auto a = dx * dx + dy * dy - k * dz * dz;
        const auto b = 2 * (dx * ox + dy * oy - k * dz * (oz - height));
        const auto c = ox * ox + oy * oy - k * (oz - height) * (oz - height);

        return solveQuadratic(a, b, c);
    }

    Float Cone::computePhi(const Point3f& hitPoint) {
        const Float phi = std::atan2(hitPoint.y, hitPoint.x);
        return phi < 0.f ? (phi + 2 * constants::Pi) : phi;
    }

    HitRecord Cone::makeHitRecord(const RayWithErrorBound& rayWithErrBound,
                                  const Point3f& hitPoint,
                                  const EFloat& t,
                                  const Float phi) const {
        const Float u = phi / phiMax;
        const Float v = hitPoint.z / height;

        const auto dpdu =
            Vector3f(-phiMax * hitPoint.y, phiMax * hitPoint.x, 0.f);
        const auto dpdv =
            Vector3f(-hitPoint.x / (1.f - v), -hitPoint.y / (1.f - v), height);

        const auto d2Pduu =
            -phiMax * phiMax * Vector3f(hitPoint.x, hitPoint.y, 0.f);
        const auto d2Pduv =
            phiMax / (1.f - v) * Vector3f(hitPoint.y, -hitPoint.x, 0.f);
        const auto d2Pdvv = Vector3f{0.f, 0.f, 0.f};

        const auto E = dot(dpdu, dpdu);
        const auto F = dot(dpdu, dpdv);
        const auto G = dot(dpdv, dpdv);
        const auto N = normalize(cross(dpdu, dpdv));
        const auto e = dot(N, d2Pduu);
        const auto f = dot(N, d2Pduv);
        const auto g = dot(N, d2Pdvv);

        const Float invEGF2 = 1 / (E * G - F * F);
        const auto dndu = Normal3f{(f * F - e * G) * invEGF2 * dpdu +
                                   (e * F - f * E) * invEGF2 * dpdv};
        const auto dndv = Normal3f{(g * F - f * G) * invEGF2 * dpdu +
                                   (f * F - g * E) * invEGF2 * dpdv};

        const auto& [ray, oErr, dErr] = rayWithErrBound;
        const auto ox = EFloat(ray.o.x, oErr.x);
        const auto oy = EFloat(ray.o.y, oErr.y);
        const auto oz = EFloat(ray.o.z, oErr.z);
        const auto dx = EFloat(ray.d.x, dErr.x);
        const auto dy = EFloat(ray.d.y, dErr.y);
        const auto dz = EFloat(ray.d.z, dErr.z);
        const auto px = ox + t * dx;
        const auto py = oy + t * dy;
        const auto pz = oz + t * dz;
        const auto pError = Vector3f{px.absoluteError(),
                                     py.absoluteError(),
                                     pz.absoluteError()};
        const auto wo = -ray.d;

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

    Float Cone::area() const {
        return radius * std::sqrt((height * height) + (radius * radius)) *
               phiMax / 2;
    }
} // namespace idragnev::pbrt