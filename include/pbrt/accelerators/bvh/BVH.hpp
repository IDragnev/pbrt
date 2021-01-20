#pragma once

#include "pbrt/core/Primitive.hpp"
#include "pbrt/core/Optional.hpp"
#include "pbrt/memory/MemoryArena.hpp"

#include <vector>
#include <memory>

namespace idragnev::pbrt::accelerators {
    namespace bvh {
        struct BuildNode;
        struct BuildTree;
        enum class SplitMethod;
    } // namespace bvh

    class BVH : public Aggregate
    {
    private:
        struct LinearBVHNode;
        struct FlattenResult;

    public:
        BVH(std::vector<std::shared_ptr<const Primitive>> primitives,
            const bvh::SplitMethod m,
            const std::uint32_t maxPrimitivesInNode = 1);
        ~BVH();

        Bounds3f worldBound() const override;

        Optional<SurfaceInteraction>
        intersect(const Ray& ray) const override;
        bool intersectP(const Ray& ray) const override;

    private:
        bvh::BuildTree buildBVHTree(const bvh::SplitMethod m,
                                    memory::MemoryArena& arena);
        FlattenResult flattenBVHTree(const bvh::BuildNode& buildNode,
                                     const std::size_t linearNodeIndex);

    private:
        std::uint32_t maxPrimitivesInNode = 1;
        std::vector<std::shared_ptr<const Primitive>> primitives;
        LinearBVHNode* nodes = nullptr;
    };
} // namespace idragnev::pbrt::accelerators