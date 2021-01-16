#pragma once

#include "BVHBuilders.hpp"

#include "pbrt/memory/MemoryArena.hpp"

namespace idragnev::pbrt::accelerators::bvh {
    class RecursiveBuilder
    {
    private:
        using PrimsVec = std::vector<std::shared_ptr<const Primitive>>;

    public:
        RecursiveBuilder() = default;
        RecursiveBuilder(const SplitMethod m, const std::size_t maxPrimsInNode)
            : splitMethod{m}
            , maxPrimitivesInNode{maxPrimsInNode} {}

        BuildResult operator()(memory::MemoryArena& arena,
                               const PrimsVec& prims) const;

    private:
        BuildTree buildSubtree(memory::MemoryArena& arena,
                               const std::span<PrimitiveInfo> primsInfoRange,
                               const PrimsVec& primitives,
                               PrimsVec& orderedPrims) const;
        BuildNode buildLeafNode(const Bounds3f& bounds,
                                const std::span<PrimitiveInfo> primsInfoRange,
                                const PrimsVec& primitives,
                                PrimsVec& orderedPrims) const;
        Optional<std::pair<BuildTree, BuildTree>>
        buildInternalNodeChildren(memory::MemoryArena& arena,
                                  const Bounds3f& rangeBounds,
                                  const Bounds3f& rangeCentroidBounds,
                                  const std::span<PrimitiveInfo> primsInfoRange,
                                  const PrimsVec& primitives,
                                  PrimsVec& orderedPrims) const;
        Optional<std::size_t> partitionPrimitivesInfo(
            const Bounds3f& rangeBounds,
            const Bounds3f& rangeCentroidBounds,
            const std::span<PrimitiveInfo> primsInfoRange) const;

    private:
        SplitMethod splitMethod = SplitMethod::SAH;
        std::size_t maxPrimitivesInNode = 1;
    };
} // namespace idragnev::pbrt::accelerators::bvh