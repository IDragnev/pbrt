#include "pbrt/cameras/EnvironmentCamera.hpp"

namespace idragnev::pbrt::cameras {
    Optional<CameraRay>
    EnvironmentCamera::generateRay(const CameraSample& sample) const {
        Ray ray = makeCameraSpaceRay(sample.pFilm);
        ray.time =
            lerp(sample.time, this->shutterOpenTime, this->shutterCloseTime);
        ray.medium = this->medium;

        return pbrt::make_optional(CameraRay{
            .ray = this->cameraToWorldTransform(ray),
            .weight = 1,
        });
    }

    Ray
    EnvironmentCamera::makeCameraSpaceRay(const Point2f& rasterPoint) const {
        using math::constants::Pi;

        const Point2i& resolution = this->film->fullResolution();

        const Float theta = Pi * rasterPoint.y / resolution.y;
        const Float phi = 2.f * Pi * rasterPoint.x / resolution.x;
        const auto rayDirection = Vector3f(std::sin(theta) * std::cos(phi),
                                           std::cos(theta),
                                           std::sin(theta) * std::sin(phi));

        return Ray(Point3f::zero(), rayDirection);
    }
} // namespace idragnev::pbrt::cameras