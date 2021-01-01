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
        BuildResult operator()(memory::MemoryArena& arena,
                               const PrimsVec& primitives) const;

    private:
        BuildTree emitLBVH(BuildNode*& buildNodes,
                           std::span<const MortonPrimitive> primsSubrange,
                           PrimsVec& orderedPrims,
                           std::atomic<std::size_t>& orderedPrimsFreePosition,
                           const int bitIndex) const;

    private:
        std::size_t maxPrimitivesInNode = 1;
        const PrimsVec* prims = nullptr;
        std::vector<PrimitiveInfo> primitivesInfo;
    };
}