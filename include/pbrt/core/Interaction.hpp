#pragma once

#include "core.hpp"
#include "math/Point3.hpp"
#include "math/Vector3.hpp"
#include "math/Normal3.hpp"
#include "Medium.hpp"

namespace idragnev::pbrt {
    class Interaction
    {
    public:
        Interaction() = default;
        Interaction(const Point3f& p,
                    const Normal3f& n,
                    const Vector3f& pError,
                    const Vector3f& wo,
                    const Float time,
                    const MediumInterface& mediumInterface)
            : time{time}
            , p{p}
            , pError{pError}
            , wo{wo}
            , n{n}
            , mediumInterface(mediumInterface) {}

        Interaction(const Point3f& p,
                    const Vector3f& wo,
                    const Float time,
                    const MediumInterface& mediumInterface)
            : time{time}
            , p{p}
            , wo{wo}
            , mediumInterface(mediumInterface) {}

        Interaction(const Point3f& p,
                    const Float time,
                    const MediumInterface& mediumInterface)
            : time{time}
            , p{p}
            , mediumInterface(mediumInterface) {}

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
        MediumInterface mediumInterface;
    };
} // namespace idragnev::pbrt
