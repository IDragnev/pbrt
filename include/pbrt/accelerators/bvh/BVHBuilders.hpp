#pragma once

#include "pbrt/core/Primitive.hpp"
#include "pbrt/core/Bounds3.hpp"
#include "pbrt/core/Point3.hpp"

#include <vector>
#include <memory>

namespace idragnev::pbrt::accelerators::bvh {
    struct PrimitiveInfo
    {
        PrimitiveInfo(const std::size_t index, const Bounds3f& bounds)
            : index(index)
            , bounds(bounds)
            , centroid(0.5f * bounds.min + 0.5f * bounds.max) {}

        std::size_t index = 0;
        Bounds3f bounds;
        Point3f centroid;
    };

    struct BuildNode
    {
        static BuildNode Leaf(const std::size_t firstPrimitiveIndex,
                              const std::size_t primitivesCount,
                              const Bounds3f& bounds) {
            return BuildNode{
                .bounds = bounds,
                .firstPrimitiveIndex = firstPrimitiveIndex,
                .primitivesCount = primitivesCount,
            };
        }

        static BuildNode Interior(const std::size_t splitAxis,
                                  BuildNode* const child0,
                                  BuildNode* const child1) {
            return BuildNode{
                .bounds = unionOf(child0->bounds, child1->bounds),
                .children = {child0, child1},
                .splitAxis = splitAxis,
            };
        }

        Bounds3f bounds;

        BuildNode* children[2] = {nullptr, nullptr};
        std::size_t splitAxis = 0;

        std::size_t firstPrimitiveIndex = 0;
        std::size_t primitivesCount = 0;
    };

    struct BuildTree
    {
        BuildNode* root = nullptr;
        std::size_t nodesCount = 0;
    };

    struct BuildResult
    {
        BuildTree tree;
        std::vector<std::shared_ptr<const Primitive>> orderedPrimitives;
    };

    enum class SplitMethod
    {
        SAH,
        HLBVH,
        Middle,
        EqualCounts,
    };
} // namespace idragnev::pbrt::bvh