#pragma once

#include "core.hpp"
#include "Point3.hpp"
#include "Vector3.hpp"
#include "Normal3.hpp"

namespace idragnev::pbrt {
    class Interaction
    {
    public:
        Interaction() = default;
        Interaction(const Point3f& p,
                    const Normal3f& n,
                    const Vector3f& pError,
                    const Vector3f& wo,
                    const Float time)
            : time{time}
            , p{p}
            , pError{pError}
            , wo{wo}
            , n{n} {}

        Interaction(const Point3f& p, const Vector3f& wo, const Float time)
            : time{time}
            , p{p}
            , wo{wo} {}

        Interaction(const Point3f& p, const Float time) : time{time}, p{p} {}

        bool isSurfaceInteraction() const noexcept { return n != Normal3f{}; }

        bool isMediumInteraction() const noexcept {
            return !isSurfaceInteraction();
        }

    public:
        Float time = 0.f;
        Point3f p;
        Vector3f pError;
        Vector3f wo;
        Normal3f n;
    };
} // namespace idragnev::pbrt
