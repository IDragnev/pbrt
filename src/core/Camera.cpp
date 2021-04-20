#include "pbrt/core/Camera.hpp"
#include "pbrt/core/geometry/Bounds2.hpp"
#include "pbrt/core/Sampling.hpp"

namespace idragnev::pbrt {
    Camera::Camera(const AnimatedTransformation& cameraToWorld,
                   const Float shutterOpenTime,
                   const Float shutterCloseTime,
                   std::unique_ptr<Film> film,
                   const Medium* medium)
        : cameraToWorldTransform(cameraToWorld)
        , shutterOpenTime(shutterOpenTime)
        , shutterCloseTime(shutterCloseTime)
        , film(std::move(film))
        , medium(medium) {
        if (cameraToWorld.hasScale()) {
            /* FIX ME WHEN LOGGING IS SUPPORTED
            Warning(
                "Scaling detected in world-to-camera transformation!\n"
                "The system has numerous assumptions, implicit and explicit,\n"
                "that this transform will have no scale factors in it.\n"
                "Proceed at your own risk; your image may have errors or\n"
                "the system may crash as a result of this.");
            */
        }
    }

    Optional<CameraRayDifferential>
    Camera::generateRayDifferential(const CameraSample& sample) const {
        return generateRay(sample)
            .map([](const CameraRay& cr) {
                return CameraRayDifferential{
                    .rayDifferential = RayDifferential(cr.ray),
                    .weight = cr.weight,
                };
            })
            .and_then([this, &sample](CameraRayDifferential&& crd)
                          -> Optional<CameraRayDifferential> {
                for (const Float eps : {0.05f, -0.05f}) {
                    CameraSample sshift = sample;
                    sshift.pFilm.x += eps;

                    if (const Optional<CameraRay> crx = generateRay(sshift);
                        crx.has_value()) {
                        const Ray& rx = crx->ray;
                        RayDifferential& rd = crd.rayDifferential;

                        rd.rxOrigin = rd.o + (rx.o - rd.o) / eps;
                        rd.rxDirection = rd.d + (rx.d - rd.d) / eps;

                        return pbrt::make_optional(crd);
                    }
                }

                return pbrt::nullopt;
            })
            .and_then([this, &sample](CameraRayDifferential&& crd)
                          -> Optional<CameraRayDifferential> {
                for (const Float eps : {0.05f, -0.05f}) {
                    CameraSample sshift = sample;
                    sshift.pFilm.y += eps;

                    if (const Optional<CameraRay> cry = generateRay(sshift);
                        cry.has_value()) {
                        const Ray& ry = cry->ray;
                        RayDifferential& rd = crd.rayDifferential;

                        rd.ryOrigin = rd.o + (ry.o - rd.o) / eps;
                        rd.ryDirection = rd.d + (ry.d - rd.d) / eps;
                        rd.hasDifferentials = true;

                        return pbrt::make_optional(crd);
                    }
                }

                return pbrt::nullopt;
            });
    }

    ProjectiveCamera::ProjectiveCamera(
        const AnimatedTransformation& cameraToWorldTransform,
        const Transformation& cameraToScreenTransform,
        const Bounds2f& screenWindow,
        const Float shutterOpenTime,
        const Float shutterCloseTime,
        const Float lensRadius,
        const Float focalDistance,
        std::unique_ptr<Film> film,
        const Medium* medium)
        : Camera(cameraToWorldTransform,
                 shutterOpenTime,
                 shutterCloseTime,
                 std::move(film),
                 medium)
        , cameraToScreenTransform(cameraToScreenTransform)
        , screenToRasterTransform(
              scaling(static_cast<Float>(this->film->fullResolution().x),
                      static_cast<Float>(this->film->fullResolution().y),
                      1.f) *
              scaling(1.f / (screenWindow.max.x - screenWindow.min.x),
                      1.f / (screenWindow.min.y - screenWindow.max.y),
                      1.f) *
              translation(
                  Vector3f(-screenWindow.min.x, -screenWindow.max.y, 0.f))
          )
        , rasterToScreenTransform(inverse(screenToRasterTransform))
        , rasterToCameraTransform(inverse(cameraToScreenTransform) * rasterToScreenTransform)
        , lensRadius(lensRadius)
        , focalDistance(focalDistance) {}

    Point2f
    ProjectiveCamera::toCameraLensPoint(const Point2f& sampleLensPoint) const {
        return this->lensRadius *
               sampling::concentricSampleDisk(sampleLensPoint);
    }
} // namespace idragnev::pbrt