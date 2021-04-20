#include "pbrt/cameras/OrthographicCamera.hpp"

namespace idragnev::pbrt::cameras {
    OrthographicCamera::OrthographicCamera(
        const AnimatedTransformation& cameraToWorldTransform,
        const Bounds2f& screenWindow,
        const Float shutterOpenTime,
        const Float shutterCloseTime,
        const Float lensRadius,
        const Float focalDistance,
        std::unique_ptr<Film> film,
        const Medium* medium)
        : ProjectiveCamera(cameraToWorldTransform,
                           orthographicTransform(0.f, 1.f),
                           screenWindow,
                           shutterOpenTime,
                           shutterCloseTime,
                           lensRadius,
                           focalDistance,
                           std::move(film),
                           medium) {
        dxCamera = this->rasterToCameraTransform(Vector3f(1.f, 0.f, 0.f));
        dyCamera = this->rasterToCameraTransform(Vector3f(0.f, 1.f, 0.f));
    }

    Optional<CameraRay>
    OrthographicCamera::generateRay(const CameraSample& sample) const {
        Ray ray = makeCameraSpaceRay(sample.pFilm, sample.pLens);
        ray.time = lerp(sample.time, this->shutterOpenTime, this->shutterCloseTime);
        ray.medium = this->medium;

        return pbrt::make_optional(CameraRay{
            .ray = this->cameraToWorldTransform(ray),
            .weight = 1.f,
        });
    }

    Optional<CameraRayDifferential>
    OrthographicCamera::generateRayDifferential(const CameraSample& sample) const {
        RayDifferential ray = makeCameraSpaceRay(sample.pFilm, sample.pLens);

        if (this->lensRadius > 0.f) {
            const Point2f pLens = toCameraLensPoint(sample.pLens);
            const Float ft = this->focalDistance / ray.d.z;

            const Point3f pFocusX = ray.o + dxCamera + (ft * Vector3f(0.f, 0.f, 1.f));
            ray.rxOrigin = Point3f(pLens.x, pLens.y, 0.f);
            ray.rxDirection = normalize(pFocusX - ray.rxOrigin);

            const Point3f pFocusY = ray.o + dyCamera + (ft * Vector3f(0.f, 0.f, 1.f));
            ray.ryOrigin = Point3f(pLens.x, pLens.y, 0.f);
            ray.ryDirection = normalize(pFocusY - ray.ryOrigin);
        }
        else {
            ray.rxOrigin = ray.o + dxCamera;
            ray.ryOrigin = ray.o + dyCamera;
            ray.ryDirection = ray.d;
            ray.rxDirection = ray.d;
        }

        ray.time = lerp(sample.time, this->shutterOpenTime, this->shutterCloseTime);
        ray.hasDifferentials = true;
        ray.medium = this->medium;

        return pbrt::make_optional(CameraRayDifferential{
            .rayDifferential = this->cameraToWorldTransform(ray),
            .weight = 1.f,
        });
    }

    Ray OrthographicCamera::makeCameraSpaceRay(const Point2f& rasterPoint,
                                               const Point2f& lensPoint) const {
        const Point3f rayOrigin = this->rasterToCameraTransform(
            Point3f(rasterPoint.x, rasterPoint.y, 0.f));
        const auto rayDirection = Vector3f(0.f, 0.f, 1.f);

        auto ray = Ray{rayOrigin, rayDirection};

        if (this->lensRadius > 0.f) {
            const Point2f pLens = toCameraLensPoint(lensPoint);
            const Float ft = this->focalDistance / ray.d.z;
            const Point3f pFocus = ray(ft);

            ray.o = Point3f(pLens.x, pLens.y, 0.f);
            ray.d = normalize(pFocus - ray.o);
        }

        return ray;
    }
} // namespace idragnev::pbrt::cameras