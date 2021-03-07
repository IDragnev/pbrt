#pragma once

#include "pbrt/core/Primitive.hpp"
#include "pbrt/core/Bounds3.hpp"
#include "pbrt/core/math/Point3.hpp"

#include <vector>
#include <memory>
#include <span>

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

    Bounds3f bounds(const std::span<const PrimitiveInfo> range);
    Bounds3f centroidBounds(const std::span<const PrimitiveInfo> range);

    // Uses the Surface Area Heuristic (SAH)
    // to find the minimum cost split position `p`.
    // Partitions the primitives at `p` only if:
    //   - primitives.size > `maxPrimitivesInNode` or
    //   - the cost of splitting at `p` < leafCost
    // where `leafCost` is the estimated SAH cost
    // for the leaf node containing this primitives.
    //
    // Returns `p` if the primitives were partitioned.
    Optional<std::size_t>
    partitionBySAH(const std::size_t splitAxis,
                   const std::span<PrimitiveInfo> primitives,
                   const Bounds3f& primitivesBounds,
                   const Bounds3f& primitivesCentroidBounds,
                   const std::size_t maxPrimitivesInNode);

    // Uses the Surface Area Heuristic (SAH)
    // to find the minimum cost split position `p`.
    //
    // Returns `p` if the primitives were partitioned.
    Optional<std::size_t>
    partitionBySAH(const std::size_t splitAxis,
                   const std::span<PrimitiveInfo> primitives,
                   const Bounds3f& primitivesBounds,
                   const Bounds3f& primitivesCentroidBounds);
} // namespace idragnev::pbrt::accelerators::bvh