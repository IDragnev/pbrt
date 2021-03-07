#pragma once

#include "Primitive.hpp"
#include "pbrt/core/transformations/AnimatedTransformation.hpp"

#include <memory>

namespace idragnev::pbrt {
    class TransformedPrimitive : public Primitive
    {
    public:
        TransformedPrimitive(std::shared_ptr<const Primitive> primitive,
                             const AnimatedTransformation& primitiveToWorld);

        Bounds3f worldBound() const override;

        Optional<SurfaceInteraction>
        intersect(const Ray& ray) const override;
        bool intersectP(const Ray& ray) const override;

        // must never be called
        const AreaLight* areaLight() const override;
        const Material* material() const override;
        void computeScatteringFunctions(
            SurfaceInteraction& interaction,
            memory::MemoryArena& arena,
            const TransportMode mode,
            const bool allowMultipleLobes) const override;

    private:
        std::shared_ptr<const Primitive> primitive;
        AnimatedTransformation primitiveToWorldTransform;
    };
} // namespace idragnev::pbrt