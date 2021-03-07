#include "pbrt/core/TransformedPrimitive.hpp"
#include "pbrt/core/transformations/Transformation.hpp"
#include "pbrt/core/SurfaceInteraction.hpp"
#include "pbrt/core/geometry/Bounds3.hpp"

#include <assert.h>

namespace idragnev::pbrt {
    TransformedPrimitive::TransformedPrimitive(
        std::shared_ptr<const Primitive> primitive,
        const AnimatedTransformation& primitiveToWorld)
        : primitive(std::move(primitive))
        , primitiveToWorldTransform(primitiveToWorld) {}

    Optional<SurfaceInteraction>
    TransformedPrimitive::intersect(const Ray& ray) const {
        const Transformation primitiveToWorldAtT =
            this->primitiveToWorldTransform.interpolate(ray.time);
        const Transformation worldToPrimitiveAtT = inverse(primitiveToWorldAtT);

        const Ray rayInPrimitiveSpace = worldToPrimitiveAtT(ray);

        return this->primitive->intersect(rayInPrimitiveSpace)
            .map([&ray, &rayInPrimitiveSpace, &primitiveToWorldAtT](
                     SurfaceInteraction&& interaction) -> SurfaceInteraction {
                ray.tMax = rayInPrimitiveSpace.tMax;

                return primitiveToWorldAtT.isIdentity() == false
                           ? primitiveToWorldAtT(interaction)
                           : std::move(interaction);
            });
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