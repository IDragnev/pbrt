#pragma once

#include "core/Primitive.hpp"
#include "core/Bounds3.hpp"
#include "core/Point3.hpp"

#include "memory/MemoryArena.hpp"

#include <vector>
#include <memory>

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
        middle,
        equalCounts,
    };

    class RecursiveBuilder
    {
    private:
        using IndicesRange = functional::IntegerRange<std::size_t>;
        using PrimsVec = std::vector<std::shared_ptr<const Primitive>>;

        struct Subtrees;

    public:
        RecursiveBuilder() = default;
        RecursiveBuilder(const SplitMethod m) : splitMethod{m} {}

        BuildResult operator()(memory::MemoryArena& arena,
                               const PrimsVec& prims);

    private:
        BuildTree buildSubtree(memory::MemoryArena& arena,
                               const IndicesRange infoIndicesRange,
                               PrimsVec& orderedPrims);
        BuildNode buildLeafNode(const Bounds3f& bounds,
                                const IndicesRange infoIndicesRange,
                                PrimsVec& orderedPrims) const;
        std::pair<BuildTree, BuildTree>
        buildInternalNodeChildren(memory::MemoryArena& arena,
                                  const Bounds3f& centroidBounds,
                                  const IndicesRange infoIndicesRange,
                                  PrimsVec& orderedPrims);

    private:
        SplitMethod splitMethod = SplitMethod::SAH;
        const PrimsVec* prims = nullptr;
        std::vector<PrimitiveInfo> primitivesInfo;
    };
} // namespace idragnev::pbrt::bvh