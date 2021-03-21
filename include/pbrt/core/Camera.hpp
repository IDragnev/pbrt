#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/Optional.hpp"
#include "pbrt/core/math/Point2.hpp"
#include "pbrt/core/geometry/Ray.hpp"
#include "pbrt/core/transformations/AnimatedTransformation.hpp"

#include <memory>

namespace idragnev::pbrt {
    struct CameraSample
    {
        Point2f pFilm;
        Point2f pLens;
        Float time = 0.f;
    };

    struct CameraRay
    {
        Ray ray;
        Float weight = 1.f;
    };

    struct CameraRayDifferential
    {
        RayDifferential rayDifferential;
        Float weight = 1.f;
    };

    // DELETE ME WHEN THE FILM CLASS IS ADDED
    class Film {};

    class Camera
    {
    public:
        Camera(const AnimatedTransformation& cameraToWorld,
               const Float shutterOpenTime,
               const Float shutterCloseTime,
               std::unique_ptr<Film> film,
               const Medium* medium);
        virtual ~Camera() = default;

        virtual Optional<CameraRay>
        generateRay(const CameraSample& sample) const = 0;
        virtual Optional<CameraRayDifferential>
        generateRayDifferential(const CameraSample& sample) const;

    private:
        AnimatedTransformation cameraToWorldTransform;
        [[maybe_unused]] Float shutterOpenTime = 0.f;
        [[maybe_unused]] Float shutterCloseTime = 0.f;
        std::unique_ptr<Film> film = nullptr;
        [[maybe_unused]] const Medium* medium = nullptr;
    };
} // namespace idragnev::pbrt