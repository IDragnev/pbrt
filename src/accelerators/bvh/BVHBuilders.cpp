#include "pbrt/accelerators/bvh/BVHBuilders.hpp"

#include <numeric>

namespace idragnev::pbrt::accelerators::bvh {
    Bounds3f bounds(const std::span<const PrimitiveInfo> range) {
        return std::accumulate(
            range.begin(),
            range.end(),
            Bounds3f{},
            [](const Bounds3f& acc, const PrimitiveInfo& info) {
                return unionOf(acc, info.bounds);
            });
    }

    Bounds3f centroidBounds(const std::span<const PrimitiveInfo> range) {
        return std::accumulate(
            range.begin(),
            range.end(),
            Bounds3f{},
            [](const Bounds3f& acc, const PrimitiveInfo& info) {
                return unionOf(acc, info.centroid);
            });
    }
} // namespace idragnev::pbrt::accelerators::bvh