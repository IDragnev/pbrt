#include "pbrt/shapes/Triangle.hpp"
#include "pbrt/core/transformations/Transformation.hpp"
#include "pbrt/core/math/Vector3.hpp"
#include "pbrt/core/math/Vector2.hpp"
#include "pbrt/core/math/Point3.hpp"
#include "pbrt/core/math/Point2.hpp"
#include "pbrt/core/math/Normal3.hpp"
#include "pbrt/core/Texture.hpp"
#include "pbrt/functional/Functional.hpp"
#include "pbrt/core/Bounds3.hpp"

namespace idragnev::pbrt::shapes {
    TriangleMesh::TriangleMesh(
        const Transformation& objectToWorld,
        const unsigned trianglesCount,
        const std::vector<std::size_t>& vertexIndices,
        const std::vector<Point3f>& vertexCoordinates,
        const std::vector<Vector3f>& vertexTangentVectors,
        const std::vector<Normal3f>& vertexNormalVectors,
        const std::vector<Point2f>& vertexUVs,
        std::shared_ptr<const Texture<Float>> alphaMask,
        std::shared_ptr<const Texture<Float>> shadowAlphaMask,
        const std::vector<std::size_t>& faceIndices)
        : trianglesCount(trianglesCount)
        , verticesCount(static_cast<unsigned>(vertexCoordinates.size()))
        , vertexIndices(vertexIndices)
        , vertexWorldCoordinates(functional::fmap(
              vertexCoordinates,
              [&objectToWorld](const Point3f& p) { return objectToWorld(p); }))
        , vertexNormalVectors(functional::fmap(
              vertexNormalVectors,
              [&objectToWorld](const Normal3f& n) { return objectToWorld(n); }))
        , vertexTangentVectors(functional::fmap(
              vertexTangentVectors,
              [&objectToWorld](const Vector3f& v) { return objectToWorld(v); }))
        , vertexUVs(vertexUVs)
        , alphaMask(std::move(alphaMask))
        , shadowAlphaMask(std::move(shadowAlphaMask))
        , faceIndices(faceIndices) {}

    Triangle::Triangle(const Transformation& objectToWorld,
                       const Transformation& worldToObject,
                       const bool reverseOrientaton,
                       std::shared_ptr<const TriangleMesh> parentMesh,
                       const unsigned number)
        : Shape(objectToWorld, worldToObject, reverseOrientaton)
        , parentMesh(std::move(parentMesh))
        // unsafe: assumes that parentMesh->vertexIndices will not change
        // after construction
        , firstVertexIndexAddress(&parentMesh->vertexIndices[3ull * number])
        , faceIndex(parentMesh->faceIndices.size() > 0
                        ? parentMesh->faceIndices[number]
                        : 0) {}

    Bounds3f Triangle::objectBound() const {
        const auto [p0, p1, p2] = verticesCoordinates();

        return unionOf(Bounds3f{(*worldToObjectTransform)(p0),
                                (*worldToObjectTransform)(p1)},
                       (*worldToObjectTransform)(p2));
    }

    Bounds3f Triangle::worldBound() const {
        const auto [p0, p1, p2] = verticesCoordinates();

        return unionOf(Bounds3f{p0, p1}, p2);
    }

    std::tuple<const Point3f&, const Point3f&, const Point3f&>
    Triangle::verticesCoordinates() const {
        const auto& vertexWorldCoordinates = parentMesh->vertexWorldCoordinates;
        const auto* const indices = firstVertexIndexAddress;

        const Point3f& p0 = vertexWorldCoordinates[indices[0]];
        const Point3f& p1 = vertexWorldCoordinates[indices[1]];
        const Point3f& p2 = vertexWorldCoordinates[indices[2]];

        return std::make_tuple(std::ref(p0), std::ref(p1), std::ref(p2));
    }

    std::array<Point2f, 3> Triangle::verticesUVs() const {
        const auto& uvs = parentMesh->vertexUVs;
        if (!uvs.empty()) {
            const auto* const indices = firstVertexIndexAddress;
            return {uvs[indices[0]], uvs[indices[1]], uvs[indices[2]]};
        }
        else {
            return {Point2f(0, 0), Point2f(1, 0), Point2f(1, 1)};
        }
    }

    Optional<HitRecord>
    Triangle::intersect(const Ray& ray, const bool testAlphaTexture) const {
        return intersectImpl<Optional<HitRecord>>(
            ray,
            testAlphaTexture,
            pbrt::nullopt,
            [this](const auto&... args) {
                return pbrt::make_optional(makeHitRecord(args...));
            });
    }

    bool Triangle::intersectP(const Ray& ray,
                              const bool testAlphaTexture) const {
        return intersectImpl<bool>(ray,
                                   testAlphaTexture,
                                   false,
                                   [](const auto&...) { return true; });
    }

    // will be instantiated only in this translation unit
    // so it is fine to define it here
    template <typename R, typename S, typename F>
    R Triangle::intersectImpl(const Ray& ray,
                              const bool testAlphaTexture,
                              F failure,
                              S success) const {
        auto [p0t, p1t, p2t, sz] = verticesInRayCoordinateSpace(ray);
        const auto [e0, e1, e2] = edgeFunctionValues(p0t, p1t, p2t);

        if ((e0 < 0.f || e1 < 0.f || e2 < 0.f) &&
            (e0 > 0.f || e1 > 0.f || e2 > 0.f)) {
            return failure;
        }

        const Float det = e0 + e1 + e2;
        if (det == 0.f) {
            return failure;
        }

        p0t.z *= sz;
        p1t.z *= sz;
        p2t.z *= sz;

        const Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
        if (det < 0 && (tScaled >= 0.f || tScaled < ray.tMax * det)) {
            return failure;
        }
        else if (det > 0 && (tScaled <= 0.f || tScaled > ray.tMax * det)) {
            return failure;
        }

        const Float invDet = 1.f / det;
        const Float b0 = e0 * invDet;
        const Float b1 = e1 * invDet;
        const Float b2 = e2 * invDet;
        const Float t = tScaled * invDet;

        const Float maxZt = maxComponent(abs(Vector3f(p0t.z, p1t.z, p2t.z)));
        const Float deltaZ = gamma(3) * maxZt;

        const Float maxXt = maxComponent(abs(Vector3f(p0t.x, p1t.x, p2t.x)));
        const Float maxYt = maxComponent(abs(Vector3f(p0t.y, p1t.y, p2t.y)));
        const Float deltaX = gamma(5) * (maxXt + maxZt);
        const Float deltaY = gamma(5) * (maxYt + maxZt);

        const Float deltaE =
            2 * (gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

        const Float maxE = maxComponent(abs(Vector3f(e0, e1, e2)));
        const Float deltaT =
            3 * (gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) *
            std::abs(invDet);

        if (t <= deltaT) {
            return failure;
        }

        const auto partialDerivatives = computePartialDerivatives();
        if (!partialDerivatives.has_value()) {
            return failure;
        }

        const auto [p0, p1, p2] = verticesCoordinates();

        const Float xAbsSum =
            (std::abs(b0 * p0.x) + std::abs(b1 * p1.x) + std::abs(b2 * p2.x));
        const Float yAbsSum =
            (std::abs(b0 * p0.y) + std::abs(b1 * p1.y) + std::abs(b2 * p2.y));
        const Float zAbsSum =
            (std::abs(b0 * p0.z) + std::abs(b1 * p1.z) + std::abs(b2 * p2.z));
        const Vector3f pError = gamma(7) * Vector3f(xAbsSum, yAbsSum, zAbsSum);

        const std::array<Point2f, 3> uv = verticesUVs();
        const Point3f pHit = b0 * p0 + b1 * p1 + b2 * p2;
        const Point2f uvHit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];

        if (testAlphaTexture && parentMesh->alphaMask != nullptr) {
            const auto localIsect = SurfaceInteraction{pHit,
                                                       Vector3f::zero(),
                                                       uvHit,
                                                       -ray.d,
                                                       partialDerivatives->dpdu,
                                                       partialDerivatives->dpdv,
                                                       Normal3f::zero(),
                                                       Normal3f::zero(),
                                                       ray.time,
                                                       this};
            if (parentMesh->alphaMask->evaluate(localIsect) == 0.f) {
                return failure;
            }
        }

        const Float barycentric[] = {b0, b1, b2};

        return success(ray,
                       t,
                       pHit,
                       pError,
                       uvHit,
                       partialDerivatives.value(),
                       barycentric);
    }

    Triangle::RayCoordinateSpaceVertices
    Triangle::verticesInRayCoordinateSpace(const Ray& ray) const {
        const auto& vertexWorldCoordinates = parentMesh->vertexWorldCoordinates;
        const auto* const indices = firstVertexIndexAddress;

        Point3f p0 = vertexWorldCoordinates[indices[0]];
        Point3f p1 = vertexWorldCoordinates[indices[1]];
        Point3f p2 = vertexWorldCoordinates[indices[2]];

        p0 -= Vector3f(ray.o);
        p1 -= Vector3f(ray.o);
        p2 -= Vector3f(ray.o);

        const auto kz = maxDimension(abs(ray.d));
        const auto kx = (kz + 1 < 3) ? kz + 1 : 0;
        const auto ky = (kx + 1 < 3) ? kx + 1 : 0;

        const Vector3f d = permute(ray.d, kx, ky, kz);
        p0 = permute(p0, kx, ky, kz);
        p1 = permute(p1, kx, ky, kz);
        p2 = permute(p2, kx, ky, kz);

        const Float sx = -d.x / d.z;
        const Float sy = -d.y / d.z;
        const Float sz = 1.f / d.z;

        // shear only the x and y dimensions for now, we can wait and
        // shear z only if the ray actually intersects the triangle
        p0.x += sx * p0.z;
        p0.y += sy * p0.z;
        p1.x += sx * p1.z;
        p1.y += sy * p1.z;
        p2.x += sx * p2.z;
        p2.y += sy * p2.z;

        RayCoordinateSpaceVertices result;
        result.p0 = p0;
        result.p1 = p1;
        result.p2 = p2;
        result.sz = sz;

        return result;
    }

    std::array<Float, 3> Triangle::edgeFunctionValues(const Point3f& p0,
                                                      const Point3f& p1,
                                                      const Point3f& p2) {
        Float e0 = p1.x * p2.y - p1.y * p2.x;
        Float e1 = p2.x * p0.y - p2.y * p0.x;
        Float e2 = p0.x * p1.y - p0.y * p1.x;

        // fall back to double precision test at triangle edges
        if constexpr (std::is_same_v<Float, float>) {
            if (e0 == 0.0f || e1 == 0.0f || e2 == 0.0f) {
                const auto p2yp1x =
                    static_cast<double>(p2.y) * static_cast<double>(p1.x);
                const auto p2xp1y =
                    static_cast<double>(p2.x) * static_cast<double>(p1.y);
                e0 = static_cast<float>(p2yp1x - p2xp1y);

                const auto p0yp2x =
                    static_cast<double>(p0.y) * static_cast<double>(p2.x);
                const auto p0xp2y =
                    static_cast<double>(p0.x) * static_cast<double>(p2.y);
                e1 = static_cast<float>(p0yp2x - p0xp2y);

                const auto p1yp0x =
                    static_cast<double>(p1.y) * static_cast<double>(p0.x);
                const auto p1xp0y =
                    static_cast<double>(p1.x) * static_cast<double>(p0.y);
                e2 = static_cast<float>(p1yp0x - p1xp0y);
            }
        }

        return {e0, e1, e2};
    }

    Optional<Triangle::PartialDerivatives>
    Triangle::computePartialDerivatives() const {
        const auto [p0, p1, p2] = verticesCoordinates();
        const std::array<Point2f, 3> uv = verticesUVs();

        const Vector2f duv02 = uv[0] - uv[2];
        const Vector2f duv12 = uv[1] - uv[2];
        const Vector3f dp02 = p0 - p2;
        const Vector3f dp12 = p1 - p2;

        const Float det = duv02[0] * duv12[1] - duv02[1] * duv12[0];
        const bool degenerateUV = std::abs(det) < 1e-8;

        Vector3f dpdu, dpdv;
        if (!degenerateUV) {
            const Float invDet = 1.f / det;
            dpdu = invDet * (duv12[1] * dp02 - duv02[1] * dp12);
            dpdv = invDet * (-duv12[0] * dp02 + duv02[0] * dp12);
        }
        if (degenerateUV || cross(dpdu, dpdv).lengthSquared() == 0.f) {
            const Vector3f ng = cross(p2 - p0, p1 - p0);
            if (ng.lengthSquared() == 0.f) {
                // The triangle is actually degenerate.
                // The intersection is bogus.
                return pbrt::nullopt;
            }

            const auto [_u, v, w] = coordinateSystem(normalize(ng));
            dpdu = v;
            dpdv = w;
        }

        return pbrt::make_optional(PartialDerivatives{dpdu, dpdv});
    }

    HitRecord
    Triangle::makeHitRecord(const Ray& ray,
                            const Float t,
                            const Point3f& hitPoint,
                            const Vector3f& pError,
                            const Point2f& hitPointUV,
                            const PartialDerivatives& partialDerivatives,
                            const Float bs[3]) const {
        auto interaction = SurfaceInteraction{hitPoint,
                                              pError,
                                              hitPointUV,
                                              -ray.d,
                                              partialDerivatives.dpdu,
                                              partialDerivatives.dpdv,
                                              Normal3f::zero(),
                                              Normal3f::zero(),
                                              ray.time,
                                              this,
                                              faceIndex};
        setShadingGeometry(interaction, bs);

        HitRecord result;
        result.interaction = interaction;
        result.t = t;

        return result;
    }

    void Triangle::setShadingGeometry(SurfaceInteraction& interaction,
                                      const Float bs[3]) const {
        const auto [p0, p1, p2] = verticesCoordinates();
        const Vector3f dp02 = p0 - p2;
        const Vector3f dp12 = p1 - p2;

        const auto n = Normal3f{normalize(cross(dp02, dp12))};
        if (reverseOrientation ^ transformSwapsHandedness) {
            interaction.n = -n;
            interaction.shading.n = -n;
        }
        else {
            interaction.n = n;
            interaction.shading.n = n;
        }

        const auto& normals = parentMesh->vertexNormalVectors;
        const auto& tangentVectors = parentMesh->vertexTangentVectors;

        if (normals.empty() && tangentVectors.empty()) {
            return;
        }

        const auto* const indices = firstVertexIndexAddress;

        Normal3f ns;
        if (!normals.empty()) {
            ns = (bs[0] * normals[indices[0]] + bs[1] * normals[indices[1]] +
                  bs[2] * normals[indices[2]]);

            if (ns.lengthSquared() > 0.f) {
                ns = normalize(ns);
            }
            else {
                ns = interaction.n;
            }
        }
        else {
            ns = interaction.n;
        }

        Vector3f ss;
        if (!tangentVectors.empty()) {
            ss = (bs[0] * tangentVectors[indices[0]] +
                  bs[1] * tangentVectors[indices[1]] +
                  bs[2] * tangentVectors[indices[2]]);
            if (ss.lengthSquared() > 0.f) {
                ss = normalize(ss);
            }
            else {
                ss = normalize(interaction.dpdu);
            }
        }
        else {
            ss = normalize(interaction.dpdu);
        }

        Vector3f ts = cross(ss, ns);
        if (ts.lengthSquared() > 0.f) {
            ts = normalize(ts);
            ss = cross(ts, ns);
        }
        else {
            const auto [_u, v, w] = coordinateSystem(Vector3f(ns));
            ss = v;
            ts = w;
        }

        if (reverseOrientation) {
            ts = -ts;
        }

        auto dndu = Normal3f::zero();
        auto dndv = Normal3f::zero();
        if (!normals.empty()) {
            const std::array<Point2f, 3> uv = verticesUVs();
            const Vector2f duv02 = uv[0] - uv[2];
            const Vector2f duv12 = uv[1] - uv[2];

            const Normal3f dn1 = normals[indices[0]] - normals[indices[2]];
            const Normal3f dn2 = normals[indices[1]] - normals[indices[2]];

            const Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
            const bool degenerateUV = std::abs(determinant) < 1e-8;

            if (degenerateUV) {
                // We can still compute dndu and dndv, with respect to the
                // same arbitrary coordinate system we use to compute dpdu
                // and dpdv when this happens. It's important to do this
                // (rather than giving up) so that ray differentials for
                // rays reflected from triangles with degenerate
                // parameterizations are still reasonable.
                const Vector3f dn =
                    cross(Vector3f(normals[indices[2]] - normals[indices[0]]),
                          Vector3f(normals[indices[1]] - normals[indices[0]]));
                if (dn.lengthSquared() == 0) {
                    dndu = Normal3f::zero();
                    dndv = Normal3f::zero();
                }
                else {
                    const auto [_u, v, w] = coordinateSystem(dn);
                    dndu = Normal3f(v);
                    dndv = Normal3f(w);
                }
            }
            else {
                const Float invDet = 1 / determinant;
                dndu = invDet * (duv12[1] * dn1 - duv02[1] * dn2);
                dndv = invDet * (-duv12[0] * dn1 + duv02[0] * dn2);
            }
        }

        interaction.setShadingGeometry(ss, ts, dndu, dndv, true);
    }

    Float Triangle::area() const {
        const auto [p0, p1, p2] = verticesCoordinates();

        return 0.5f * cross(p1 - p0, p2 - p0).length();
    }

    std::vector<std::shared_ptr<Shape>>
    createTriangleMesh(const Transformation& objectToWorld,
                       const Transformation& worldToObject,
                       const bool reverseOrientation,
                       const unsigned trianglesCount,
                       const std::vector<std::size_t>& vertexIndices,
                       const std::vector<Point3f>& vertexCoordinates,
                       const std::vector<Vector3f>& vertexTangentVectors,
                       const std::vector<Normal3f>& vertexNormalVectors,
                       const std::vector<Point2f>& vertexUVs,
                       std::shared_ptr<const Texture<Float>> alphaMask,
                       std::shared_ptr<const Texture<Float>> shadowAlphaMask,
                       const std::vector<std::size_t>& faceIndices) {
        using functional::IntegerRange;
        const auto mesh =
            std::make_shared<TriangleMesh>(objectToWorld,
                                           trianglesCount,
                                           vertexIndices,
                                           vertexCoordinates,
                                           vertexTangentVectors,
                                           vertexNormalVectors,
                                           vertexUVs,
                                           std::move(alphaMask),
                                           std::move(shadowAlphaMask),
                                           faceIndices);
        return functional::fmap<std::vector>(
            IntegerRange{0u, trianglesCount},
            [&](const unsigned i) -> std::shared_ptr<Shape> {
                return std::make_shared<Triangle>(objectToWorld,
                                                  worldToObject,
                                                  reverseOrientation,
                                                  mesh,
                                                  i);
            });
    }
} // namespace idragnev::pbrt::shapes