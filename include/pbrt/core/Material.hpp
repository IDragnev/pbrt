#pragma once

#include "core.hpp"
#include "pbrt/memory/MemoryArena.hpp"

namespace idragnev::pbrt {
    enum class TransportMode
    {
        radiance,
        importance,
    };

    class Material
    {
    public:
        virtual ~Material() = default;

        virtual void
        computeScatteringFunctions(SurfaceInteraction& interaction,
                                   memory::MemoryArena& arena,
                                   const TransportMode mode,
                                   const bool allowMultipleLobes) const = 0;
    };
} // namespace idragnev::pbrt