#pragma once

#include "BVHBuilders.hpp"

#include "pbrt/core/Optional.hpp"
#include "pbrt/memory/MemoryArena.hpp"

#include <array>

namespace idragnev::pbrt::functional {
    template <typename T>
    class IntegerRange;
}

namespace idragnev::pbrt::accelerators::bvh {
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
        Optional<std::pair<BuildTree, BuildTree>>
        buildInternalNodeChildren(memory::MemoryArena& arena,
                                  const Bounds3f& rangeBounds,
                                  const Bounds3f& rangeCentroidBounds,
                                  const IndicesRange infoIndicesRange,
                                  PrimsVec& orderedPrims);
        Optional<std::size_t>
        partitionPrimitivesInfo(const Bounds3f& rangeBounds,
                                const Bounds3f& rangeCentroidBounds,
                                const IndicesRange infoIndicesRange);
        std::size_t partitionPrimitivesInfoInEqualSubsets(
            const std::size_t splitAxis,
            const IndicesRange infoIndicesRange);
        Optional<std::size_t> partitionPrimitivesInfoAtAxisMiddle(
            const Bounds3f& rangeCentroidBounds,
            const IndicesRange infoIndicesRange);
        Optional<std::size_t>
        partitionPrimitivesInfoBySAH(const Bounds3f& rangeBounds,
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
} // namespace idragnev::pbrt::accelerators::bvh