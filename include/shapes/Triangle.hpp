#pragma once

#include "core/core.hpp"
#include "core/Shape.hpp"

#include <vector>
#include <memory>

namespace idragnev::pbrt {
    struct TriangleMesh
    {
        TriangleMesh(
            const Transformation& objectToWorld,
            const int trianglesCount,
            const std::vector<int>& vertexIndices,
            const std::vector<Point3f>& vertexCoordinates,
            const std::vector<Vector3f>& vertexTangentVectors,
            const std::vector<Normal3f>& vertexNormalVectors,
            const std::vector<Point2f>& vertexUVs,
            const std::shared_ptr<const Texture<Float>>& alphaMask,
            const std::shared_ptr<const Texture<Float>>& shadowAlphaMask,
            const std::vector<int>& faceIndices);

        int trianglesCount = 0;
        int verticesCount = 0;
        std::vector<int> vertexIndices;
        std::vector<Point3f> vertexWorldCoordinates;
        std::vector<Normal3f> vertexNormalVectors;
        std::vector<Vector3f> vertexTangentVectors;
        std::vector<Point2f> vertexUVs;
        std::shared_ptr<const Texture<Float>> alphaMask;
        std::shared_ptr<const Texture<Float>> shadowAlphaMask;
        std::vector<int> faceIndices;
    };

    class Triangle : public Shape
    {
    public:
        Triangle(const Transformation& objectToWorld,
                 const Transformation& worldToObject,
                 const bool reverseOrientaton,
                 const std::shared_ptr<const TriangleMesh>& parentMesh,
                 const int number);

        Bounds3f objectBound() const override;
        Bounds3f worldBound() const override;

        std::optional<HitRecord>
        intersect(const Ray& ray, const bool testAlphaTexture) const override;

        bool intersectP(const Ray& ray,
                        const bool testAlphaTexture) const override;

        Float area() const override;

    private:
        std::shared_ptr<const TriangleMesh> parentMesh = nullptr;
        const int* firstVertexIndexAddress = nullptr;
        int faceIndex = 0;
    };

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
        const std::vector<int>& faceIndices);
} // namespace idragnev::pbrt