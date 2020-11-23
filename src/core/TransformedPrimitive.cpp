#include "core/TransformedPrimitive.hpp"
#include "core/Transformation.hpp"
#include "core/SurfaceInteraction.hpp"
#include "core/Bounds3.hpp"

#include <assert.h>

namespace idragnev::pbrt {
    TransformedPrimitive::TransformedPrimitive(
        std::shared_ptr<const Primitive> primitive,
        const AnimatedTransformation& primitiveToWorld)
        : primitive(std::move(primitive))
        , primitiveToWorldTransform(primitiveToWorld) {}

    std::optional<SurfaceInteraction>
    TransformedPrimitive::intersect(const Ray& ray) const {
        const Transformation primitiveToWorldAtT =
            this->primitiveToWorldTransform.interpolate(ray.time);
        const Transformation worldToPrimitiveAtT = inverse(primitiveToWorldAtT);

        const Ray rayInPrimitiveSpace = worldToPrimitiveAtT(ray);

        // FIX ME WHEN A PROPER OPTIONAL IS USED
        if (auto optInteraction = primitive->intersect(rayInPrimitiveSpace);
            optInteraction.has_value())
        {
            ray.tMax = rayInPrimitiveSpace.tMax;

            if (!primitiveToWorldAtT.isIdentity()) {
                return std::make_optional(primitiveToWorldAtT(*optInteraction));
            }
            else {
                return optInteraction;
            }
        }

        return std::nullopt;
    }

    bool TransformedPrimitive::intersectP(const Ray& ray) const {
        const Transformation primitiveToWorldAtT =
            this->primitiveToWorldTransform.interpolate(ray.time);
        const Transformation worldToPrimitiveAtT = inverse(primitiveToWorldAtT);

        return primitive->intersectP(worldToPrimitiveAtT(ray));
    }

    Bounds3f TransformedPrimitive::worldBound() const {
        return primitiveToWorldTransform.motionBounds(primitive->worldBound());
    }

    const AreaLight* TransformedPrimitive::areaLight() const {
        assert(false);
        return nullptr;
    }

    const Material* TransformedPrimitive::material() const {
        assert(false);
        return nullptr;
    }

    void TransformedPrimitive::computeScatteringFunctions(
        SurfaceInteraction&,
        memory::MemoryArena&,
        const TransportMode,
        const bool) const {
        assert(false);
    }
} // namespace idragnev::pbrt