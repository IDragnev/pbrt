#pragma once

#include "core.hpp"

namespace idragnev::pbrt {
    template <typename T>
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual T evaluate(const SurfaceInteraction&) const = 0;
    };
} // namespace idragnev::pbrt