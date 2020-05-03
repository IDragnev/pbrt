#pragma once

#include "core.hpp"
#include "SurfaceInteraction.hpp"

#include <optional>

namespace idragnev::pbrt {
    struct HitRecord
    {
        Float t = constants::Infinity;
        SurfaceInteraction interaction;
    };

    class Shape
    {
    public:
        Shape(const Transformation& objectToWorld, 
            const Transformation& worldToObject,
            const bool reverseOrientation) noexcept;
        virtual ~Shape() = default;

        virtual Bounds3f objectBound() const = 0;
        virtual Bounds3f worldBound() const;

        virtual std::optional<HitRecord> intersect(const Ray& ray, const bool testAlphaTexture = true) const = 0;
        virtual bool intersectP(const Ray& ray, const bool testAlphaTexture = true) const;

        virtual Float area() const = 0;

    public:
        const Transformation* objectToWorldTransform = nullptr;
        const Transformation* worldToObjectTransform = nullptr;
        const bool reverseOrientation = false;
        const bool transformSwapsHandedness = false;
    };
} //namespace idragnev::pbrt