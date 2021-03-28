#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/Optional.hpp"
#include "pbrt/core/math/Point2.hpp"
#include "pbrt/core/geometry/Ray.hpp"
#include "pbrt/core/transformations/Transformation.hpp"
#include "pbrt/core/transformations/AnimatedTransformation.hpp"

#include <memory>

namespace idragnev::pbrt {
    struct CameraSample
    {
        Point2f pFilm;
        Point2f pLens;
        // in [0, 1), the generated ray's time
        // relative to the camera's shutter open/close time
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
    class Film {
    public:
        const Point2i& fullResolution() const;
    };

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

    protected:
        AnimatedTransformation cameraToWorldTransform;
        Float shutterOpenTime = 0.f;
        Float shutterCloseTime = 0.f;
        std::unique_ptr<Film> film = nullptr;
        const Medium* medium = nullptr;
    };

    class ProjectiveCamera : public Camera
    {
    public:
        ProjectiveCamera(const AnimatedTransformation& cameraToWorldTransform,
                         const Transformation& cameraToScreenTransform,
                         const Bounds2f& screenWindow,
                         const Float shutterOpenTime,
                         const Float shutterCloseTime,
                         const Float lensRadius,
                         const Float focalDistance,
                         std::unique_ptr<Film> film,
                         const Medium* medium);

    protected:
        Transformation cameraToScreenTransform;
        Transformation screenToRasterTransform;
        Transformation rasterToScreenTransform;
        Transformation rasterToCameraTransform;
        Float lensRadius = 0.f;
        Float focalDistance = 0.f;
    };
} // namespace idragnev::pbrt