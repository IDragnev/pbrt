#include "pbrt/cameras/PerspectiveCamera.hpp"

namespace idragnev::pbrt::cameras {
    PerspectiveCamera::PerspectiveCamera(
        const AnimatedTransformation& cameraToWorld,
        const Bounds2f& screenWindow,
        const Float shutterOpenTime,
        const Float shutterCloseTime,
        const Float lensRadius,
        const Float focalDistance,
        const Float fov,
        std::unique_ptr<Film> film,
        const Medium* medium)
        : ProjectiveCamera(cameraToWorld,
                           perspectiveTransform(fov, 1e-2f, 1000.f),
                           screenWindow,
                           shutterOpenTime,
                           shutterCloseTime,
                           lensRadius,
                           focalDistance,
                           std::move(film),
                           medium) {
        const Point3f cameraRaysOrigin =
            this->rasterToCameraTransform(Point3f::zero());
        dxCamera = (this->rasterToCameraTransform(Point3f(1.f, 0.f, 0.f)) -
                    cameraRaysOrigin);
        dyCamera = (this->rasterToCameraTransform(Point3f(0.f, 1.f, 0.f)) -
                    cameraRaysOrigin);

        const Point2i res = this->film->fullResolution();
        Point3f pMin = this->rasterToCameraTransform(Point3f::zero());
        Point3f pMax =
            this->rasterToCameraTransform(Point3f(res.x, res.y, 0.f));
        pMin /= pMin.z;
        pMax /= pMax.z;

        this->A = std::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));
    }

    Optional<CameraRay>
    PerspectiveCamera::generateRay(const CameraSample& sample) const {
        Ray ray = rasterPointToCameraSpaceRay(sample.pFilm);

        //if (this->lensRadius > 0.f) {
        //    const Point2f pLens = this->lensRadius * concentricSampleDisk(sample.pLens);
        //    const Float ft = this->focalDistance / ray.d.z;
        //    const Point3f pFocus = ray(ft);

        //    ray.o = Point3f(pLens.x, pLens.y, 0.f);
        //    ray.d = normalize(pFocus - ray.o);
        //}

        ray.medium = this->medium;
        ray.time =
            lerp(sample.time, this->shutterOpenTime, this->shutterCloseTime);

        return pbrt::make_optional(CameraRay{
            .ray = this->cameraToWorldTransform(ray),
            .weight = 1.f,
        });
    }

    Optional<CameraRayDifferential> PerspectiveCamera::generateRayDifferential(
        const CameraSample& sample) const {
        RayDifferential ray = rasterPointToCameraSpaceRay(sample.pFilm);

        // if (this->lensRadius > 0.f) {
        //    const Point2f pLens = this->lensRadius * concentricSampleDisk(sample.pLens);
        //    const Float ft = this->focalDistance / ray.d.z;
        //    const Point3f pFocus = ray(ft);

        //    ray.o = Point3f(pLens.x, pLens.y, 0.f);
        //    ray.d = normalize(pFocus - ray.o);
        //}

        if (const Point3f pCamera = this->rasterToCameraTransform(
                Point3f(sample.pFilm.x, sample.pFilm.y, 0.f));
            this->lensRadius > 0.f)
        {
        //    const Point2f pLens = this->lensRadius * concentricSampleDisk(sample.pLens);
        //    const Vector3f dx = normalize(Vector3f(pCamera + dxCamera));
        //    const Float ftx = this->focalDistance / dx.z;
        //    const Point3f pFocusX = Point3f::zero() + (ftx * dx);
        //    ray.rxOrigin = Point3f(pLens.x, pLens.y, 0);
        //    ray.rxDirection = normalize(pFocusX - ray.rxOrigin);

        //    const Vector3f dy = normalize(Vector3f(pCamera + dyCamera));
        //    const Float fty = this->focalDistance / dy.z;
        //    const Point3f pFocusY = Point3f::zero() + (fty * dy);
        //    ray.ryOrigin = Point3f(pLens.x, pLens.y, 0.f);
        //    ray.ryDirection = normalize(pFocusY - ray.ryOrigin);
        }
        else {
            ray.rxOrigin = ray.o;
            ray.ryOrigin = ray.o;
            ray.rxDirection = normalize(Vector3f(pCamera) + dxCamera);
            ray.ryDirection = normalize(Vector3f(pCamera) + dyCamera);
        }

        ray.time =
            lerp(sample.time, this->shutterOpenTime, this->shutterCloseTime);
        ray.hasDifferentials = true;
        ray.medium = this->medium;

        return pbrt::make_optional(CameraRayDifferential{
            .rayDifferential = this->cameraToWorldTransform(ray),
            .weight = 1.f,
        });
    }

    Ray PerspectiveCamera::rasterPointToCameraSpaceRay(const Point2f& p) const {
        const Point3f rayOrigin = Point3f::zero();
        const Point3f pCamera =
            this->rasterToCameraTransform(Point3f(p.x, p.y, 0.f));
        const Vector3f rayDirection = normalize(Vector3f(pCamera));

        return Ray(rayOrigin, rayDirection);
    }
} // namespace idragnev::pbrt::cameras