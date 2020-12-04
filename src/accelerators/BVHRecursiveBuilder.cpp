#include "accelerators/BVHBuilders.hpp"
#include "functional/Functional.hpp"

namespace idragnev::pbrt::accelerators::bvh {
    struct BuildNode
    {
        static BuildNode Leaf(const std::size_t firstPrimitiveIndex,
                              const std::size_t primitivesCount,
                              const Bounds3f& bounds) {
            BuildNode result{};
            result.firstPrimitiveIndex = firstPrimitiveIndex;
            result.primitivesCount = primitivesCount;
            result.bounds = bounds;

            return result;
        }

        static BuildNode Interior(const std::size_t splitAxis,
                                  BuildNode* const child0,
                                  BuildNode* const child1) {
            BuildNode result{};
            result.children[0] = child0;
            result.children[1] = child1;
            result.splitAxis = splitAxis;
            result.bounds = unionOf(child0->bounds, child1->bounds);

            return result;
        }

        Bounds3f bounds;

        BuildNode* children[2] = {nullptr, nullptr};
        std::size_t splitAxis = 0;

        std::size_t firstPrimitiveIndex = 0;
        std::size_t primitivesCount = 0;
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
        BuildTree result{};
        result.root = arena.alloc<BuildNode>();

        const Bounds3f subtreeBounds = functional::foldl(
            infoIndicesRange,
            Bounds3f{},
            [this](const Bounds3f& acc, const std::size_t i) {
                return unionOf(acc, this->primitivesInfo[i].bounds);
            });

        if (const auto primitivesCount = infoIndicesRange.size();
            primitivesCount == 1)
        {
            result.nodesCount = primitivesCount;
            *result.root = buildLeafNode(subtreeBounds, infoIndicesRange, orderedPrims);
        }
        else {
            const Bounds3f centroidBounds = functional::foldl(
                infoIndicesRange,
                Bounds3f{},
                [this](const Bounds3f& acc, const std::size_t i) {
                    return unionOf(acc, this->primitivesInfo[i].centroid);
                });
            const auto dim = centroidBounds.maximumExtent();

            if (centroidBounds.max[dim] == centroidBounds.min[dim]) {
                result.nodesCount = primitivesCount;
                *result.root = buildLeafNode(subtreeBounds,
                                             infoIndicesRange,
                                             orderedPrims);
            }
            else {
                const auto [left, right] =
                    buildInternalNodeChildren(arena,
                                              centroidBounds,
                                              infoIndicesRange,
                                              orderedPrims);
                const auto axis = dim;

                result.nodesCount = left.nodesCount + right.nodesCount;
                *result.root = BuildNode::Interior(axis, left.root, right.root);
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

    std::pair<BuildTree, BuildTree> RecursiveBuilder::buildInternalNodeChildren(
        memory::MemoryArena& arena,
        const Bounds3f& centroidBounds,
        const IndicesRange infoIndicesRange,
        PrimsVec& orderedPrims) {
        partitionPrimitivesInfo(centroidBounds, infoIndicesRange);

        const auto indicesMid =
            (infoIndicesRange.first() + infoIndicesRange.last()) / 2;
        const auto left =
            buildSubtree(arena,
                         IndicesRange{infoIndicesRange.first(), indicesMid},
                         orderedPrims);
        const auto right =
            buildSubtree(arena,
                         IndicesRange{indicesMid, infoIndicesRange.last()},
                         orderedPrims);

        return std::make_pair(left, right);
    }

    void RecursiveBuilder::partitionPrimitivesInfo(
        const Bounds3f& centroidBounds,
        const IndicesRange infoIndicesRange) {
        const auto dim = centroidBounds.maximumExtent();
        const auto primsInfoBegin = this->primitivesInfo.begin();

        switch (this->splitMethod) {
            case SplitMethod::Middle: {
                const Float boundsMid =
                    (centroidBounds.min[dim] + centroidBounds.max[dim]) / 2.f;
                const auto firstPrimPos =
                    primsInfoBegin + infoIndicesRange.first();
                const auto lastPrimPos =
                    primsInfoBegin + infoIndicesRange.last();
                const auto midPrimPos = std::partition(
                    firstPrimPos,
                    lastPrimPos,
                    [dim, boundsMid](const PrimitiveInfo& info) {
                        return info.centroid[dim] < boundsMid;
                    });

                // If the partition was successful, break the switch.
                // Otherwise fall through to try with EqualCounts.
                if (midPrimPos != firstPrimPos && midPrimPos != lastPrimPos) {
                    break;
                }
            }
                [[fallthrough]];
            case SplitMethod::EqualCounts: {
                const auto indicesMid =
                    (infoIndicesRange.first() + infoIndicesRange.last()) / 2;
                std::nth_element(
                    primsInfoBegin + infoIndicesRange.first(),
                    primsInfoBegin + indicesMid,
                    primsInfoBegin + infoIndicesRange.last(),
                    [dim](const PrimitiveInfo& a, const PrimitiveInfo& b) {
                        return a.centroid[dim] < b.centroid[dim];
                    });
            } break;
            case SplitMethod::SAH:
            default: {
                // TODO
            } break;
        }
    }
} // namespace idragnev::pbrt::accelerators::bvh