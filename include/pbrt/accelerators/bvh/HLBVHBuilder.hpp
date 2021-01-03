#pragma once

#include "BVHBuilders.hpp"

#include "pbrt/memory/MemoryArena.hpp"

#include <span>
#include <atomic>

namespace idragnev::pbrt::accelerators::bvh {
    struct MortonPrimitive;

    class HLBVHBuilder
    {
    private:
        using PrimsVec = std::vector<std::shared_ptr<const Primitive>>;

    public:
        HLBVHBuilder() = default;
        HLBVHBuilder(const std::size_t maxPrimsInNode) noexcept
            : maxPrimitivesInNode(maxPrimsInNode) {}

        BuildResult operator()(memory::MemoryArena& arena,
                               const PrimsVec& primitives);

    private:
        BuildTree emitLBVH(BuildNode*& unusedNodes,
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
    };
} // namespace idragnev::pbrt::accelerators::bvh