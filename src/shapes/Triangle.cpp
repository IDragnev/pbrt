#include "shapes/Triangle.hpp"
#include "core/Transformation.hpp"
#include "core/Vector3.hpp"
#include "core/Point3.hpp"
#include "core/Point2.hpp"
#include "core/Texture.hpp"
#include "core/Normal3.hpp"
#include "core/Functional.hpp"

namespace idragnev::pbrt {
    TriangleMesh::TriangleMesh(
        const Transformation& objectToWorld,
        const int trianglesCount,
        const std::vector<int>& vertexIndices,
        const std::vector<Point3f>& vertexPositions,
        const std::vector<Vector3f>& vertexTangentVectors,
        const std::vector<Normal3f>& vertexNormalVectors,
        const std::vector<Point2f>& vertexUVs,
        const std::shared_ptr<const Texture<Float>>& alphaMask,
        const std::shared_ptr<const Texture<Float>>& shadowAlphaMask,
        const std::vector<int>& faceIndices)
        : trianglesCount(trianglesCount)
        , verticesCount(vertexPositions.size())
        , vertexIndices(vertexIndices)
        , vertexPositions(fmap(
              vertexPositions,
              [objectToWorld](const Point3f& p) { return objectToWorld(p); }))
        , vertexNormalVectors(fmap(
              vertexNormalVectors,
              [objectToWorld](const Normal3f& n) { return objectToWorld(n); }))
        , vertexTangentVectors(fmap(
              vertexTangentVectors,
              [objectToWorld](const Vector3f& v) { return objectToWorld(v); }))
        , vertexUVs(vertexUVs)
        , alphaMask(alphaMask)
        , shadowAlphaMask(shadowAlphaMask)
        , faceIndices(faceIndices) {}
} // namespace idragnev::pbrt