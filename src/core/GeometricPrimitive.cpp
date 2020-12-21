#include "pbrt/core/GeometricPrimitive.hpp"
#include "pbrt/core/Shape.hpp"
#include "pbrt/core/Bounds3.hpp"
#include "pbrt/core/SurfaceInteraction.hpp"
#include "pbrt/core/Material.hpp"

#include <assert.h>

namespace idragnev::pbrt {
    GeometricPrimitive::GeometricPrimitive(
        std::shared_ptr<const Shape> shape,
        std::shared_ptr<const Material> material,
        std::shared_ptr<const AreaLight> areaLight,
        const MediumInterface& mediumInterface)
        : _shape(std::move(shape))
        , _material(std::move(material))
        , _areaLight(std::move(areaLight))
        , _mediumInterface(mediumInterface) {}

    Bounds3f GeometricPrimitive::worldBound() const {
        return _shape->worldBound();
    }

    bool GeometricPrimitive::intersectP(const Ray& ray) const {
        return _shape->intersectP(ray);
    }

    Optional<SurfaceInteraction>
    GeometricPrimitive::intersect(const Ray& ray) const {
        return _shape->intersect(ray).map(
            [&ray, this](HitRecord&& hitRecord) -> SurfaceInteraction {
                ray.tMax = hitRecord.t;

                auto& interaction = hitRecord.interaction;
                interaction.primitive = this;
                interaction.mediumInterface =
                    _mediumInterface.isMediumTransition()
                        ? _mediumInterface
                        : MediumInterface{ray.medium};

                return std::move(interaction);
            });
    }

    void GeometricPrimitive::computeScatteringFunctions(
        SurfaceInteraction& interaction,
        memory::MemoryArena& arena,
        const TransportMode mode,
        const bool allowMultipleLobes) const {
        if (_material != nullptr)
            _material->computeScatteringFunctions(interaction,
                                                  arena,
                                                  mode,
                                                  allowMultipleLobes);
        assert(dot(interaction.n, interaction.shading.n) >= 0.f);
    }

    const AreaLight* GeometricPrimitive::areaLight() const {
        return _areaLight.get();
    }

    const Material* GeometricPrimitive::material() const {
        return _material.get();
    }
} // namespace idragnev::pbrt