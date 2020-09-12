#pragma once

#include "core/core.hpp"

#include <vector>
#include <memory>

namespace idragnev::pbrt {
    struct TriangleMesh
    {
        TriangleMesh(
            const Transformation& objectToWorld,
            const int trianglesCount,
            const std::vector<int>& vertexIndices,
            const std::vector<Point3f>& vertexPositions,
            const std::vector<Vector3f>& vertexTangentVectors,
            const std::vector<Normal3f>& vertexNormalVectors,
            const std::vector<Point2f>& vertexUVs,
            const std::shared_ptr<const Texture<Float>>& alphaMask,
            const std::shared_ptr<const Texture<Float>>& shadowAlphaMask,
            const std::vector<int>& faceIndices);

        int trianglesCount;
        int verticesCount;
        std::vector<int> vertexIndices;
        std::vector<Point3f> vertexPositions;
        std::vector<Normal3f> vertexNormalVectors;
        std::vector<Vector3f> vertexTangentVectors;
        std::vector<Point2f> vertexUVs;
        std::shared_ptr<const Texture<Float>> alphaMask;
        std::shared_ptr<const Texture<Float>> shadowAlphaMask;
        std::vector<int> faceIndices;
    };
} // namespace idragnev::pbrt