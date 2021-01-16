#include "pbrt/accelerators/bvh/RecursiveBuilder.hpp"
#include "pbrt/functional/Functional.hpp"

namespace idragnev::pbrt::accelerators::bvh {
    std::size_t partitionPrimitivesInfoInEqualSubsets(
        const std::size_t splitAxis,
        const std::span<PrimitiveInfo> primsInfoRange);
    Optional<std::size_t> partitionPrimitivesInfoAtAxisMiddle(
        const Bounds3f& rangeCentroidBounds,
        const std::span<PrimitiveInfo> primsInfoRange);
    Optional<std::size_t>
    partitionPrimitivesInfoBySAH(const Bounds3f& rangeBounds,
                                 const Bounds3f& rangeCentroidBounds,
                                 const std::span<PrimitiveInfo> primsInfoRange,
                                 const std::size_t maxPrimsInNode);

    BuildResult RecursiveBuilder::operator()(memory::MemoryArena& arena,
                                             const PrimsVec& primitives) const {
        BuildResult result{};
        if (primitives.empty()) {
            return result;
        }

        std::vector<PrimitiveInfo> primitivesInfo = functional::fmapIndexed(
            primitives,
            [](const auto& primitive, const std::size_t i) {
                return PrimitiveInfo{i, primitive->worldBound()};
            });

        result.orderedPrimitives.reserve(primitives.size());
        result.tree = buildSubtree(
            arena,
            std::span{primitivesInfo.begin(), primitivesInfo.size()},
            primitives,
            result.orderedPrimitives);

        return result;
    }

    BuildTree RecursiveBuilder::buildSubtree(
        memory::MemoryArena& arena,
        const std::span<PrimitiveInfo> primsInfoRange,
        const PrimsVec& primitives,
        PrimsVec& orderedPrims) const {
        BuildTree result{
            .root = arena.alloc<BuildNode>(),
        };

        const Bounds3f rangeBounds = bounds(primsInfoRange);

        if (primsInfoRange.size() == 1) {
            result.nodesCount = 1;
            *result.root = buildLeafNode(rangeBounds,
                                         primsInfoRange,
                                         primitives,
                                         orderedPrims);
        }
        else {
            const Bounds3f rangeCentroidBounds = centroidBounds(primsInfoRange);
            const auto splitAxis = rangeCentroidBounds.maximumExtent();

            if (rangeCentroidBounds.max[splitAxis] ==
                rangeCentroidBounds.min[splitAxis]) {
                result.nodesCount = 1;
                *result.root = buildLeafNode(rangeBounds,
                                             primsInfoRange,
                                             primitives,
                                             orderedPrims);
            }
            else {
                const auto subtrees =
                    buildInternalNodeChildren(arena,
                                              rangeBounds,
                                              rangeCentroidBounds,
                                              primsInfoRange,
                                              primitives,
                                              orderedPrims);
                if (subtrees.has_value()) {
                    const auto [left, right] = subtrees.value();

                    result.nodesCount = left.nodesCount + right.nodesCount + 1;
                    *result.root =
                        BuildNode::Interior(splitAxis, left.root, right.root);
                }
                else { // failed to split the primitives into two subtrees
                    result.nodesCount = 1;
                    *result.root = buildLeafNode(rangeBounds,
                                                 primsInfoRange,
                                                 primitives,
                                                 orderedPrims);
                }
            }
        }

        return result;
    }

    BuildNode RecursiveBuilder::buildLeafNode(
        const Bounds3f& bounds,
        const std::span<PrimitiveInfo> primsInfoRange,
        const PrimsVec& primitives,
        PrimsVec& orderedPrims) const {
        const auto firstPrimIndex = orderedPrims.size();
        const auto primitivesCount = primsInfoRange.size();

        for (const PrimitiveInfo& pi : primsInfoRange) {
            orderedPrims.push_back(primitives[pi.index]);
        }

        return BuildNode::Leaf(firstPrimIndex, primitivesCount, bounds);
    }

    Optional<std::pair<BuildTree, BuildTree>>
    RecursiveBuilder::buildInternalNodeChildren(
        memory::MemoryArena& arena,
        const Bounds3f& rangeBounds,
        const Bounds3f& rangeCentroidBounds,
        const std::span<PrimitiveInfo> primsInfoRange,
        const PrimsVec& primitives,
        PrimsVec& orderedPrims) const {
        const auto splitPosition = partitionPrimitivesInfo(rangeBounds,
                                                           rangeCentroidBounds,
                                                           primsInfoRange);
        return splitPosition.map([this,
                                  &orderedPrims,
                                  &primsInfoRange,
                                  &primitives,
                                  &arena](const std::size_t splitPos) {
            const BuildTree left =
                buildSubtree(arena,
                             primsInfoRange.first(splitPos),
                             primitives,
                             orderedPrims);
            const BuildTree right =
                buildSubtree(arena,
                             primsInfoRange.subspan(splitPos),
                             primitives,
                             orderedPrims);

            return std::make_pair(left, right);
        });
    }

    Optional<std::size_t> RecursiveBuilder::partitionPrimitivesInfo(
        const Bounds3f& rangeBounds,
        const Bounds3f& rangeCentroidBounds,
        const std::span<PrimitiveInfo> primsInfoRange) const {
        switch (this->splitMethod) {
            case SplitMethod::Middle: {
                const auto splitPos =
                    partitionPrimitivesInfoAtAxisMiddle(rangeCentroidBounds,
                                                        primsInfoRange);
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
                                                          primsInfoRange);
                return pbrt::make_optional(splitPosition);
            } break;
            case SplitMethod::SAH:
            default: {
                return partitionPrimitivesInfoBySAH(rangeBounds,
                                                    rangeCentroidBounds,
                                                    primsInfoRange,
                                                    this->maxPrimitivesInNode);
            } break;
        }
    }

    // Partially sort the primitivesInfo subrange according to their
    // position relative to the specified axis' middle, producing
    // two equally sized subsets.
    std::size_t partitionPrimitivesInfoInEqualSubsets(
        const std::size_t splitAxis,
        const std::span<PrimitiveInfo> primsInfoRange) {
        const auto mid = primsInfoRange.size() / 2;

        std::nth_element(
            primsInfoRange.begin(),
            primsInfoRange.begin() + mid,
            primsInfoRange.end(),
            [dim = splitAxis](const PrimitiveInfo& a, const PrimitiveInfo& b) {
                return a.centroid[dim] < b.centroid[dim];
            });

        return mid;
    }

    // Partition the primitiveInfo subrange according to
    // the primitives' positions relative to the middle of
    // the range's controid bounds' maximum extent
    Optional<std::size_t> partitionPrimitivesInfoAtAxisMiddle(
        const Bounds3f& rangeCentroidBounds,
        const std::span<PrimitiveInfo> primsInfoRange) {
        const auto dim = rangeCentroidBounds.maximumExtent();
        const Float axisMid =
            (rangeCentroidBounds.min[dim] + rangeCentroidBounds.max[dim]) / 2.f;

        const auto midPrimPos =
            std::partition(primsInfoRange.begin(),
                           primsInfoRange.end(),
                           [dim, axisMid](const PrimitiveInfo& info) {
                               return info.centroid[dim] < axisMid;
                           });

        if (midPrimPos != primsInfoRange.begin() &&
            midPrimPos != primsInfoRange.end()) {
            const auto splitPosition = midPrimPos - primsInfoRange.begin();
            return pbrt::make_optional(static_cast<std::size_t>(splitPosition));
        }

        return pbrt::nullopt;
    }

    // Partition the primitiveInfo subrange using the 
    // Surface Area Heuristic (SAH)
    Optional<std::size_t>
    partitionPrimitivesInfoBySAH(const Bounds3f& rangeBounds,
                                 const Bounds3f& rangeCentroidBounds,
                                 const std::span<PrimitiveInfo> primsInfoRange,
                                 const std::size_t maxPrimsInNode) {
        constexpr std::size_t N_PRIMITIVES_LOWER_BOUND = 5;

        const std::size_t splitAxis = rangeCentroidBounds.maximumExtent();

        if (primsInfoRange.size() < N_PRIMITIVES_LOWER_BOUND) {
            const std::size_t splitPosition =
                partitionPrimitivesInfoInEqualSubsets(splitAxis,
                                                      primsInfoRange);
            return pbrt::make_optional(splitPosition);
        }
        else {
            return partitionBySAH(splitAxis,
                                  primsInfoRange,
                                  rangeBounds,
                                  rangeCentroidBounds,
                                  maxPrimsInNode);
        }
    }
} // namespace idragnev::pbrt::accelerators::bvh