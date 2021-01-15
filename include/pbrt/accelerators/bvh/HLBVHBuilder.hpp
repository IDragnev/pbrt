#pragma once

#include "BVHBuilders.hpp"

#include "pbrt/memory/MemoryArena.hpp"

#include <span>
#include <atomic>

namespace idragnev::pbrt::accelerators::bvh {
    struct MortonPrimitive;
    struct LBVHTreelet;

    class HLBVHBuilder
    {
    private:
        using PrimsVec = std::vector<std::shared_ptr<const Primitive>>;

        struct LowerLevels
        {
            std::vector<BuildNode*> roots;
            std::size_t nodesCount = 0;
            PrimsVec orderedPrimitives;
        };

        struct UpperLevelTree
        {
            BuildNode* root = nullptr;
            std::size_t allocatedNodesCount = 0;
        };

    public:
        HLBVHBuilder() = default;
        HLBVHBuilder(const std::size_t maxPrimsInNode) noexcept
            : maxPrimitivesInNode(maxPrimsInNode) {}

        BuildResult operator()(memory::MemoryArena& arena,
                               const PrimsVec& primitives);

    private:
        BuildResult buildBVH(memory::MemoryArena& arena,
                             LowerLevels&& lowerLevels) const;
        UpperLevelTree buildUpperLevelTree(
            memory::MemoryArena& arena,
            const std::span<PrimitiveInfo> treeletRootsInfosRange,
            const std::vector<BuildNode*>& treeletRoots) const;
        LowerLevels buildLowerLevels(std::vector<LBVHTreelet>&& treelets) const;
        BuildTree emitLBVH(BuildNode*& buildNodes,
                           const std::span<const MortonPrimitive> primsSubrange,
                           PrimsVec& orderedPrims,
                           std::atomic<std::size_t>& orderedPrimsFreePosition,
                           const int splitBit) const;
        BuildNode
        makeLeafNode(const std::span<const MortonPrimitive> primsRange,
                     const std::size_t firstPrimIndex,
                     PrimsVec& orderedPrims) const;

    private:
        std::size_t maxPrimitivesInNode = 1;
        const PrimsVec* prims = nullptr;
        std::vector<PrimitiveInfo> primitivesInfo;
        std::vector<MortonPrimitive> mortonPrimInfos;
    };
} // namespace idragnev::pbrt::accelerators::bvh