#include "pbrt/core/primitive/Primitive.hpp"

#include <assert.h>

namespace idragnev::pbrt {
    const AreaLight* Aggregate::areaLight() const {
        assert(false);
        return nullptr;
    }

    const Material* Aggregate::material() const {
        assert(false);
        return nullptr;
    }

    void Aggregate::computeScatteringFunctions(SurfaceInteraction&,
                                               memory::MemoryArena&,
                                               const TransportMode,
                                               const bool) const {
        assert(false);
    }
} // namespace idragnev::pbrt