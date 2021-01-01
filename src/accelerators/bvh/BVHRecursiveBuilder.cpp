#include "pbrt/accelerators/bvh/RecursiveBuilder.hpp"
#include "pbrt/functional/Functional.hpp"

namespace idragnev::pbrt::accelerators::bvh {
    struct RecursiveBuilder::SAHBucket
    {
        std::uint32_t primitivesCount = 0;
        Bounds3f bounds;
    };

    BuildResult RecursiveBuilder::operator()(memory::MemoryArena& arena,
                                             const PrimsVec& primitives) {
        using functional::ScopedFn;
        [[maybe_unused]] const auto cleanUp =
            ScopedFn{[&builder = *this]() noexcept {
                builder = {};
            }};

        BuildResult result{};
        if (primitives.empty()) {
            return result;
        }

        this->prims = &primitives;
        this->primitivesInfo = functional::fmapIndexed(
            primitives,
            [](const auto& primitive, const std::size_t i) {
                return PrimitiveInfo{i, primitive->worldBound()};
            });

        result.orderedPrimitives.reserve(primitives.size());
        result.tree = buildSubtree(arena,
                                   IndicesRange{0, this->primitivesInfo.size()},
                                   result.orderedPrimitives);

        return result;
    }

    BuildTree
    RecursiveBuilder::buildSubtree(memory::MemoryArena& arena,
                                   const IndicesRange infoIndicesRange,
                                   PrimsVec& orderedPrims) {
        BuildTree result{
            .root = arena.alloc<BuildNode>(),
        };

        const Bounds3f rangeBounds = functional::foldl(
            infoIndicesRange,
            Bounds3f{},
            [this](const Bounds3f& acc, const std::size_t i) {
                return unionOf(acc, this->primitivesInfo[i].bounds);
            });

        if (const auto primitivesCount = infoIndicesRange.size();
            primitivesCount == 1)
        {
            result.nodesCount = primitivesCount;
            *result.root = buildLeafNode(rangeBounds, infoIndicesRange, orderedPrims);
        }
        else {
            const Bounds3f rangeCentroidBounds = functional::foldl(
                infoIndicesRange,
                Bounds3f{},
                [this](const Bounds3f& acc, const std::size_t i) {
                    return unionOf(acc, this->primitivesInfo[i].centroid);
                });
            const auto splitAxis = rangeCentroidBounds.maximumExtent();

            if (rangeCentroidBounds.max[splitAxis] ==
                rangeCentroidBounds.min[splitAxis]) {
                result.nodesCount = primitivesCount;
                *result.root = buildLeafNode(rangeBounds,
                                             infoIndicesRange,
                                             orderedPrims);
            }
            else {
                const auto subtrees = 
                    buildInternalNodeChildren(arena,
                                              rangeBounds,
                                              rangeCentroidBounds,
                                              infoIndicesRange,
                                              orderedPrims);
                if (subtrees.has_value()) {
                    const auto [left, right] = subtrees.value();

                    result.nodesCount = left.nodesCount + right.nodesCount;
                    *result.root =
                        BuildNode::Interior(splitAxis, left.root, right.root);
                }
                else { // failed to split the primitives into two subtrees
                    result.nodesCount = primitivesCount;
                    *result.root = buildLeafNode(rangeBounds,
                                                 infoIndicesRange,
                                                 orderedPrims);
                }
            }
        }

        return result;
    }

    BuildNode
    RecursiveBuilder::buildLeafNode(const Bounds3f& bounds,
                                    const IndicesRange infoIndicesRange,
                                    PrimsVec& orderedPrims) const {
        const auto firstPrimIndex = orderedPrims.size();

        const auto& primitives = *(this->prims);
        for (auto i = infoIndicesRange.first(); i < infoIndicesRange.last();
             ++i) {
            const auto index = this->primitivesInfo[i].index;
            orderedPrims.push_back(primitives[index]);
        }

        const auto primitivesCount = infoIndicesRange.size();

        return BuildNode::Leaf(firstPrimIndex, primitivesCount, bounds);
    }

    Optional<std::pair<BuildTree, BuildTree>>
    RecursiveBuilder::buildInternalNodeChildren(
        memory::MemoryArena& arena,
        const Bounds3f& rangeBounds,
        const Bounds3f& rangeCentroidBounds,
        const IndicesRange infoIndicesRange,
        PrimsVec& orderedPrims) {
        const auto splitPosition = partitionPrimitivesInfo(rangeBounds,
                                                           rangeCentroidBounds,
                                                           infoIndicesRange);
        return splitPosition.map([this,
                                  &orderedPrims,
                                  &infoIndicesRange,
                                  &arena](const std::size_t splitPos) {
            const BuildTree left =
                buildSubtree(arena,
                             IndicesRange{infoIndicesRange.first(), splitPos},
                             orderedPrims);
            const BuildTree right =
                buildSubtree(arena,
                             IndicesRange{splitPos, infoIndicesRange.last()},
                             orderedPrims);

            return std::make_pair(left, right);
        });
    }

    Optional<std::size_t> RecursiveBuilder::partitionPrimitivesInfo(
        const Bounds3f& rangeBounds,
        const Bounds3f& rangeCentroidBounds,
        const IndicesRange infoIndicesRange) {
        switch (this->splitMethod) {
            case SplitMethod::Middle: {
                const auto splitPos =
                    partitionPrimitivesInfoAtAxisMiddle(rangeCentroidBounds,
                                                        infoIndicesRange);
                if (splitPos.has_value()) {
                    return splitPos;
                }
                // If the split was unsuccessful, try EqualCounts.
            }
                [[fallthrough]];
            case SplitMethod::EqualCounts: {
                const auto splitAxis = rangeCentroidBounds.maximumExtent();
                const auto splitPosition =
                    partitionPrimitivesInfoInEqualSubsets(splitAxis,
                                                          infoIndicesRange);
                return pbrt::make_optional(splitPosition);
            } break;
            case SplitMethod::SAH:
            default: {
                return partitionPrimitivesInfoBySAH(rangeBounds,
                                                    rangeCentroidBounds,
                                                    infoIndicesRange);
            } break;
        }
    }

    // Partially sort the primitivesInfo subrange according to their
    // position relative to the specified axis' middle, producing
    // two equally sized subsets.
    std::size_t RecursiveBuilder::partitionPrimitivesInfoInEqualSubsets(
        const std::size_t splitAxis,
        const IndicesRange infoIndicesRange) {
        const auto primsInfoBegin = this->primitivesInfo.begin();
        const auto indicesMid =
            (infoIndicesRange.first() + infoIndicesRange.last()) / 2;

        std::nth_element(
            primsInfoBegin + infoIndicesRange.first(),
            primsInfoBegin + indicesMid,
            primsInfoBegin + infoIndicesRange.last(),
            [dim = splitAxis](const PrimitiveInfo& a, const PrimitiveInfo& b) {
                return a.centroid[dim] < b.centroid[dim];
            });

        return indicesMid;
    }

    // Partition the primitiveInfo subrange according to 
    // the primitives' positions relative to the middle of 
    // the range's controid bounds' maximum extent
    Optional<std::size_t>
    RecursiveBuilder::partitionPrimitivesInfoAtAxisMiddle(
        const Bounds3f& rangeCentroidBounds,
        const IndicesRange infoIndicesRange) {
        const auto dim = rangeCentroidBounds.maximumExtent();
        const Float boundsMid =
            (rangeCentroidBounds.min[dim] + rangeCentroidBounds.max[dim]) / 2.f;

        const auto primsInfoBegin = this->primitivesInfo.begin();
        const auto firstPrimPos = primsInfoBegin + infoIndicesRange.first();
        const auto lastPrimPos = primsInfoBegin + infoIndicesRange.last();
        const auto midPrimPos =
            std::partition(firstPrimPos,
                           lastPrimPos,
                           [dim, boundsMid](const PrimitiveInfo& info) {
                               return info.centroid[dim] < boundsMid;
                           });

        if (midPrimPos != firstPrimPos && midPrimPos != lastPrimPos) {
            const auto splitPosition = midPrimPos - primsInfoBegin;
            return pbrt::make_optional(static_cast<std::size_t>(splitPosition));
        }

        return pbrt::nullopt;
    }

    // Partition the primitiveInfo subrange using the 
    // Surface Area Heuristic (SAH)
    Optional<std::size_t>
    bvh::RecursiveBuilder::partitionPrimitivesInfoBySAH(
        const Bounds3f& rangeBounds,
        const Bounds3f& rangeCentroidBounds,
        const IndicesRange infoIndicesRange) {
        constexpr std::size_t N_PRIMITIVES_LOWER_BOUND = 5;

        const std::size_t splitAxis = rangeCentroidBounds.maximumExtent();
        const std::size_t primitivesCount = infoIndicesRange.size();

        if (primitivesCount < N_PRIMITIVES_LOWER_BOUND) {
            const std::size_t splitPosition =
                partitionPrimitivesInfoInEqualSubsets(splitAxis,
                                                      infoIndicesRange);
            return pbrt::make_optional(splitPosition);
        }
        else {
            const auto buckets = splitToSAHBuckets(splitAxis,
                                                   rangeCentroidBounds,
                                                   infoIndicesRange);
            const auto splitCosts = computeSplitCosts(rangeBounds, buckets);
            const auto minCostPosition =
                std::min_element(splitCosts.cbegin(), splitCosts.cend());
            const Float minCost =
                minCostPosition != splitCosts.cend() ? (*minCostPosition) : 0.f;
            const std::size_t minCostSplitBucketIndex =
                minCostPosition != splitCosts.cend()
                    ? static_cast<std::size_t>(minCostPosition -
                                               splitCosts.cbegin())
                    : 0;

            const Float leafCost = static_cast<Float>(primitivesCount);
            if (primitivesCount > this->maxPrimitivesInNode ||
                minCost < leafCost) {
                const auto primsInfoBegin = this->primitivesInfo.begin();
                const auto pmid = std::partition(
                    primsInfoBegin + infoIndicesRange.first(),
                    primsInfoBegin + infoIndicesRange.last(),
                    [&rangeCentroidBounds,
                     splitAxis,
                     &buckets,
                     minCostSplitBucketIndex](const PrimitiveInfo& info) {
                        const std::size_t index =
                            bucketIndex(info,
                                        rangeCentroidBounds,
                                        buckets,
                                        splitAxis);

                        return index <= minCostSplitBucketIndex;
                    });

                const auto splitPosition = pmid - primsInfoBegin;
                return pbrt::make_optional(static_cast<std::size_t>(splitPosition));
            }
        }

        return pbrt::nullopt;
    }

    auto RecursiveBuilder::splitToSAHBuckets(
        const std::size_t splitAxis,
        const Bounds3f& rangeCentroidBounds,
        const IndicesRange infoIndicesRange) const -> SAHBucketsArray {
        SAHBucketsArray buckets{};

        std::for_each(
            this->primitivesInfo.begin() + infoIndicesRange.first(),
            this->primitivesInfo.begin() + infoIndicesRange.last(),
            [&rangeCentroidBounds, &buckets, splitAxis](
                const PrimitiveInfo& info) {
                const std::size_t index =
                    bucketIndex(info, rangeCentroidBounds, buckets, splitAxis);

                buckets[index].primitivesCount++;
                buckets[index].bounds =
                    unionOf(buckets[index].bounds, info.bounds);
            });

        return buckets;
    }

    RecursiveBuilder::SAHSplitCostsArray
    RecursiveBuilder::computeSplitCosts(const Bounds3f& rangeBounds,
                                        const SAHBucketsArray& buckets) const {
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
                set.size += buckets[i].primitivesCount;
            }
            return set;
        };

        SAHSplitCostsArray splitCosts{};
        const Float rangeBoundsSurfaceArea = rangeBounds.surfaceArea();
        for (std::size_t i = 0; i < splitCosts.size(); ++i) {
            const PrimitiveSet set1 = makeSetFromBuckets(0u, i + 1);
            const PrimitiveSet set2 = makeSetFromBuckets(i + 1, buckets.size());

            splitCosts[i] = 1 + (set1.size * set1.bounds.surfaceArea() +
                                 set2.size * set2.bounds.surfaceArea()) /
                                    rangeBoundsSurfaceArea;
        }

        return splitCosts;
    }

    std::size_t
    RecursiveBuilder::bucketIndex(const PrimitiveInfo& info,
                                  const Bounds3f& rangeCentroidBounds,
                                  const SAHBucketsArray& buckets,
                                  const std::size_t splitAxis) {
        const Vector3f centroidOffset =
            rangeCentroidBounds.offset(info.centroid);
        auto index = static_cast<std::size_t>(
            centroidOffset[splitAxis] * static_cast<Float>(buckets.size()));
        index = clamp(index, 0u, buckets.size() - 1u);

        return index;
    }
} // namespace idragnev::pbrt::accelerators::bvh