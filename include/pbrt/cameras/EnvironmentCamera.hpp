#pragma once

#include "pbrt/core/Camera.hpp"

namespace idragnev::pbrt::cameras {
    class EnvironmentCamera : public Camera
    {
    public:
        using Camera::Camera;

        Optional<CameraRay>
        generateRay(const CameraSample& sample) const override;

    private:
        Ray makeCameraSpaceRay(const Point2f& rasterPoint) const;
    };
} // namespace idragnev::pbrt::cameras