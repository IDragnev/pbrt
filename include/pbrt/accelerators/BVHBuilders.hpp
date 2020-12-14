#pragma once

#include "core/Primitive.hpp"
#include "core/Bounds3.hpp"
#include "core/Point3.hpp"

#include "memory/MemoryArena.hpp"

#include <vector>
#include <memory>
#include <array>
#include <optional>

namespace idragnev::pbrt::functional {
    template <typename T>
    class IntegerRange;
}

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

    struct BuildNode;

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

    class RecursiveBuilder
    {
    private:
        using IndicesRange = functional::IntegerRange<std::size_t>;
        using PrimsVec = std::vector<std::shared_ptr<const Primitive>>;

        struct SAHBucket;
        static inline constexpr std::size_t SAH_BUCKETS_COUNT = 12;
        using SAHBucketsArray = std::array<SAHBucket, SAH_BUCKETS_COUNT>;
        using SAHSplitCostsArray = std::array<Float, SAH_BUCKETS_COUNT - 1>;

    public:
        RecursiveBuilder() = default;
        RecursiveBuilder(const SplitMethod m, const std::size_t maxPrimsInNode)
            : splitMethod{m}
            , maxPrimitivesInNode{maxPrimsInNode} {}

        BuildResult operator()(memory::MemoryArena& arena,
                               const PrimsVec& prims);

    private:
        BuildTree buildSubtree(memory::MemoryArena& arena,
                               const IndicesRange infoIndicesRange,
                               PrimsVec& orderedPrims);
        BuildNode buildLeafNode(const Bounds3f& bounds,
                                const IndicesRange infoIndicesRange,
                                PrimsVec& orderedPrims) const;
        std::optional<std::pair<BuildTree, BuildTree>>
        buildInternalNodeChildren(memory::MemoryArena& arena,
                                  const Bounds3f& rangeBounds,
                                  const Bounds3f& rangeCentroidBounds,
                                  const IndicesRange infoIndicesRange,
                                  PrimsVec& orderedPrims);
        std::optional<std::size_t>
        partitionPrimitivesInfo(const Bounds3f& rangeBounds,
                                const Bounds3f& rangeCentroidBounds,
                                const IndicesRange infoIndicesRange);
        std::size_t partitionPrimitivesInfoInEqualSubsets(
            const std::size_t splitAxis,
            const IndicesRange infoIndicesRange);
        std::optional<std::size_t> partitionPrimitivesInfoAtAxisMiddle(
            const Bounds3f& rangeCentroidBounds,
            const IndicesRange infoIndicesRange);
        std::optional<std::size_t> partitionPrimitivesInfoBySAH(
            const Bounds3f& rangeBounds,
            const Bounds3f& rangeCentroidBounds,
            const IndicesRange infoIndicesRange);
        SAHBucketsArray
        splitToSAHBuckets(const std::size_t splitAxis,
                          const Bounds3f& rangeCentroidBounds,
                          const IndicesRange infoIndicesRange) const;
        SAHSplitCostsArray
        computeSplitCosts(const Bounds3f& rangeBounds,
                          const SAHBucketsArray& buckets) const;

        static std::size_t bucketIndex(const PrimitiveInfo& info,
                                       const Bounds3f& rangeCentroidBounds,
                                       const SAHBucketsArray& buckets,
                                       const std::size_t splitAxis);

    private:
        SplitMethod splitMethod = SplitMethod::SAH;
        std::size_t maxPrimitivesInNode = 1;
        const PrimsVec* prims = nullptr;
        std::vector<PrimitiveInfo> primitivesInfo;
    };
} // namespace idragnev::pbrt::bvh