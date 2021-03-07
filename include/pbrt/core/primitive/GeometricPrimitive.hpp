#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/Medium.hpp"
#include "pbrt/core/primitive/Primitive.hpp"

#include <memory>

namespace idragnev::pbrt {
    class GeometricPrimitive : public Primitive
    {
    public:
        GeometricPrimitive(std::shared_ptr<const Shape> shape,
                           std::shared_ptr<const Material> material,
                           std::shared_ptr<const AreaLight> areaLight,
                           const MediumInterface& mediumInterface);

        Bounds3f worldBound() const override;

        Optional<SurfaceInteraction>
        intersect(const Ray& ray) const override;
        bool intersectP(const Ray& ray) const override;

        const AreaLight* areaLight() const override;
        const Material* material() const override;
        void computeScatteringFunctions(
            SurfaceInteraction& interaction,
            memory::MemoryArena& arena,
            const TransportMode mode,
            const bool allowMultipleLobes) const override;

    private:
        std::shared_ptr<const Shape> _shape = nullptr;
        std::shared_ptr<const Material> _material = nullptr;
        std::shared_ptr<const AreaLight> _areaLight = nullptr;
        MediumInterface _mediumInterface{};
    };
} // namespace idragnev::pbrt