#include "shapes/Triangle.hpp"
#include "core/Transformation.hpp"
#include "core/Vector3.hpp"
#include "core/Point3.hpp"
#include "core/Point2.hpp"
#include "core/Texture.hpp"
#include "core/Normal3.hpp"
#include "core/Functional.hpp"
#include "core/Bounds3.hpp"

namespace idragnev::pbrt {
    TriangleMesh::TriangleMesh(
        const Transformation& objectToWorld,
        const int trianglesCount,
        const std::vector<int>& vertexIndices,
        const std::vector<Point3f>& vertexCoordinates,
        const std::vector<Vector3f>& vertexTangentVectors,
        const std::vector<Normal3f>& vertexNormalVectors,
        const std::vector<Point2f>& vertexUVs,
        const std::shared_ptr<const Texture<Float>>& alphaMask,
        const std::shared_ptr<const Texture<Float>>& shadowAlphaMask,
        const std::vector<int>& faceIndices)
        : trianglesCount(trianglesCount)
        , verticesCount(vertexCoordinates.size())
        , vertexIndices(vertexIndices)
        , vertexWorldCoordinates(fmap(
              vertexCoordinates,
              [&objectToWorld](const Point3f& p) { return objectToWorld(p); }))
        , vertexNormalVectors(fmap(
              vertexNormalVectors,
              [&objectToWorld](const Normal3f& n) { return objectToWorld(n); }))
        , vertexTangentVectors(fmap(
              vertexTangentVectors,
              [&objectToWorld](const Vector3f& v) { return objectToWorld(v); }))
        , vertexUVs(vertexUVs)
        , alphaMask(alphaMask)
        , shadowAlphaMask(shadowAlphaMask)
        , faceIndices(faceIndices) {}

    Triangle::Triangle(const Transformation& objectToWorld,
                       const Transformation& worldToObject,
                       const bool reverseOrientaton,
                       const std::shared_ptr<const TriangleMesh>& parentMesh,
                       const int number)
        : Shape(objectToWorld, worldToObject, reverseOrientaton)
        , parentMesh(parentMesh)
        , firstVertexIndexAddress(&parentMesh->vertexIndices[3 * number])
        , faceIndex(parentMesh->faceIndices.size() > 0
                        ? parentMesh->faceIndices[number]
                        : 0) {}

    Bounds3f Triangle::objectBound() const {
        const auto& vertexWorldCoordinates = parentMesh->vertexWorldCoordinates;
        const auto* const indices = firstVertexIndexAddress;

        const Point3f& p0 = vertexWorldCoordinates[indices[0]];
        const Point3f& p1 = vertexWorldCoordinates[indices[1]];
        const Point3f& p2 = vertexWorldCoordinates[indices[2]];

        return unionOf(Bounds3f{(*worldToObjectTransform)(p0),
                                (*worldToObjectTransform)(p1)},
                       (*worldToObjectTransform)(p2));
    }

    Bounds3f Triangle::worldBound() const {
        const auto& vertexWorldCoordinates = parentMesh->vertexWorldCoordinates;
        const auto* const indices = firstVertexIndexAddress;

        const Point3f& p0 = vertexWorldCoordinates[indices[0]];
        const Point3f& p1 = vertexWorldCoordinates[indices[1]];
        const Point3f& p2 = vertexWorldCoordinates[indices[2]];

        return unionOf(Bounds3f{p0, p1}, p2);
    }

    std::optional<HitRecord>
    Triangle::intersect(const Ray& ray, const bool testAlphaTexture) const {
        return std::nullopt;
    }

    bool Triangle::intersectP(const Ray& ray,
                              const bool testAlphaTexture) const {
        return false;
    }

    Float Triangle::area() const { return 0.f; }

    std::vector<std::shared_ptr<Shape>> createTriangleMesh(
        const Transformation& objectToWorld,
        const Transformation& worldToObject,
        const bool reverseOrientation,
        const int trianglesCount,
        const std::vector<int>& vertexIndices,
        const std::vector<Point3f>& vertexCoordinates,
        const std::vector<Vector3f>& vertexTangentVectors,
        const std::vector<Normal3f>& vertexNormalVectors,
        const std::vector<Point2f>& vertexUVs,
        const std::shared_ptr<const Texture<Float>>& alphaMask,
        const std::shared_ptr<const Texture<Float>>& shadowAlphaMask,
        const std::vector<int>& faceIndices) {
        const auto mesh = std::make_shared<TriangleMesh>(objectToWorld,
                                                         trianglesCount,
                                                         vertexIndices,
                                                         vertexCoordinates,
                                                         vertexTangentVectors,
                                                         vertexNormalVectors,
                                                         vertexUVs,
                                                         alphaMask,
                                                         shadowAlphaMask,
                                                         faceIndices);
        return mapIntegerRange<std::vector>(
            0,
            trianglesCount,
            [&](const int i) -> std::shared_ptr<Shape> {
                return std::make_shared<Triangle>(objectToWorld,
                                                  worldToObject,
                                                  reverseOrientation,
                                                  mesh,
                                                  i);
            });
    }
} // namespace idragnev::pbrt