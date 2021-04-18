#pragma once

#include "pbrt/core/Camera.hpp"

namespace idragnev::pbrt::cameras {
    class OrthographicCamera : public ProjectiveCamera
    {
    public:
        OrthographicCamera(const AnimatedTransformation& cameraToWorld,
                           const Bounds2f& screenWindow,
                           const Float shutterOpenTime,
                           const Float shutterCloseTime,
                           const Float lensRadius,
                           const Float focalDistance,
                           std::unique_ptr<Film> film,
                           const Medium* medium);

        Optional<CameraRay>
        generateRay(const CameraSample& sample) const override;
        Optional<CameraRayDifferential>
        generateRayDifferential(const CameraSample& sample) const override;

    private:
        Ray rasterPointToCameraSpaceRay(const Point2f& rasterPoint) const;

    private:
        Vector3f dxCamera;
        Vector3f dyCamera;
    };
} // namespace idragnev::pbrt::cameras