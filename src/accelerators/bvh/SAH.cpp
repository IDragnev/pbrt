#include "pbrt/accelerators/bvh/BVHBuilders.hpp"

#include <array>

namespace idragnev::pbrt::accelerators::bvh {
    struct Bucket
    {
        std::uint32_t size = 0;
        Bounds3f bounds;
    };

    struct BestSplit
    {
        Float cost = 0.f;
        std::size_t splitBucketIndex = 0;
    };

    static inline constexpr std::size_t BUCKETS_COUNT = 12;
    using BucketsArray = std::array<Bucket, BUCKETS_COUNT>;
    using SplitCostsArray = std::array<Float, BUCKETS_COUNT - 1>;

    BucketsArray
    splitToBuckets(const std::size_t splitAxis,
                   const Bounds3f& centroidBounds,
                   const std::span<const PrimitiveInfo> primitives);
    BestSplit findBestSplit(const Bounds3f& primitivesBounds,
                            const BucketsArray& buckets);
    SplitCostsArray computeSplitCosts(const Bounds3f& primitivesBounds,
                                      const BucketsArray& buckets);
    std::size_t bucketIndex(const PrimitiveInfo& info,
                            const Bounds3f& centroidBounds,
                            const BucketsArray& buckets,
                            const std::size_t splitAxis);

    Optional<std::size_t>
    partitionBySAH(const std::size_t splitAxis,
                   const std::span<PrimitiveInfo> primitives,
                   const Bounds3f& primitivesBounds,
                   const Bounds3f& primitivesCentroidBounds,
                   const std::size_t maxPrimitivesInNode) {
        const auto buckets =
            splitToBuckets(splitAxis, primitivesCentroidBounds, primitives);
        const auto split = findBestSplit(primitivesBounds, buckets);

        const Float leafCost = static_cast<Float>(primitives.size());
        if (primitives.size() > maxPrimitivesInNode || split.cost < leafCost) {
            const auto splitPos = std::partition(
                primitives.begin(),
                primitives.end(),
                [&primitivesCentroidBounds, splitAxis, &buckets, &split](
                    const PrimitiveInfo& info) {
                    const std::size_t index =
                        bucketIndex(info,
                                    primitivesCentroidBounds,
                                    buckets,
                                    splitAxis);

                    return index <= split.splitBucketIndex;
                });
            const auto splitPosition = splitPos - primitives.begin();

            return pbrt::make_optional(static_cast<std::size_t>(splitPosition));
        }
        else {
            return pbrt::nullopt;
        }
    }

    BucketsArray
    splitToBuckets(const std::size_t splitAxis,
                   const Bounds3f& centroidBounds,
                   const std::span<const PrimitiveInfo> primitives) {
        BucketsArray buckets{};

        for (const PrimitiveInfo& info : primitives) {
            const std::size_t index =
                bucketIndex(info, centroidBounds, buckets, splitAxis);

            buckets[index].size += 1;
            buckets[index].bounds = unionOf(buckets[index].bounds, info.bounds);
        }

        return buckets;
    }

    BestSplit findBestSplit(const Bounds3f& primitivesBounds,
                            const BucketsArray& buckets) {
        const auto splitCosts = computeSplitCosts(primitivesBounds, buckets);
        const auto minCostPosition =
            std::min_element(splitCosts.cbegin(), splitCosts.cend());

        return BestSplit{
            .cost = *minCostPosition,
            .splitBucketIndex =
                static_cast<std::size_t>(minCostPosition - splitCosts.cbegin()),
        };
    }

    SplitCostsArray computeSplitCosts(const Bounds3f& primitivesBounds,
                                      const BucketsArray& buckets) {
        struct PrimitiveSet
        {
            Bounds3f bounds;
            std::size_t size = 0;
        };

        const auto makeSetFromBuckets = [&buckets](const std::size_t from,
                                                   const std::size_t to) {
            PrimitiveSet set{};
            for (auto i = from; i < to; ++i) {
                set.bounds = unionOf(set.bounds, buckets[i].bounds);
                set.size += buckets[i].size;
            }
            return set;
        };

        SplitCostsArray splitCosts{};
        const Float primsBoundsSurfaceArea = primitivesBounds.surfaceArea();
        for (std::size_t i = 0; i < splitCosts.size(); ++i) {
            const PrimitiveSet set1 = makeSetFromBuckets(0u, i + 1);
            const PrimitiveSet set2 = makeSetFromBuckets(i + 1, buckets.size());

            splitCosts[i] = 1 + (set1.size * set1.bounds.surfaceArea() +
                                 set2.size * set2.bounds.surfaceArea()) /
                                    primsBoundsSurfaceArea;
        }

        return splitCosts;
    }

    std::size_t bucketIndex(const PrimitiveInfo& info,
                            const Bounds3f& centroidBounds,
                            const BucketsArray& buckets,
                            const std::size_t splitAxis) {
        const Vector3f centroidOffset = centroidBounds.offset(info.centroid);
        auto index = static_cast<std::size_t>(
            centroidOffset[splitAxis] * static_cast<Float>(buckets.size()));
        index = clamp(index, 0u, buckets.size() - 1u);

        return index;
    }
} // namespace idragnev::pbrt::accelerators::bvh