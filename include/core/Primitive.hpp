#pragma once

#include "core.hpp"
#include "memory/MemoryArena.hpp"

#include <optional>

namespace idragnev::pbrt {
    class Primitive
    {
    public:
        virtual ~Primitive() = default;

        virtual Bounds3f WorldBound() const = 0;

        virtual std::optional<SurfaceInteraction>
        intersect(const Ray& r) const = 0;
        virtual bool intersectP(const Ray& r) const = 0;

        virtual const AreaLight* areaLight() const = 0;
        virtual const Material* material() const = 0;
        virtual void
        computeScatteringFunctions(SurfaceInteraction& interactiion,
                                   memory::MemoryArena& arena,
                                   const TransportMode mode,
                                   const bool allowMultipleLobes) const = 0;
    };
} // namespace idragnev::pbrt