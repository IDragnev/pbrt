#include "pbrt/core/Film.hpp"

namespace idragnev::pbrt {
    Film::Film(const Point2i& resolution,
               const Bounds2f& cropWindow,
               std::unique_ptr<const Filter> filt,
               Float diagonal,
               const std::string& filename,
               Float scale,
               Float maxSampleLuminance)
        : fullResolution(resolution)
        , diagonal(Float(diagonal * 0.001))
        , filter(std::move(filt))
        , filename(filename)
        // clang-format off
        , croppedPixelBounds(Bounds2i(
              Point2i(static_cast<int>(std::ceil(fullResolution.x * cropWindow.min.x)),
                      static_cast<int>(std::ceil(fullResolution.y * cropWindow.min.y))),
              Point2i(static_cast<int>(std::ceil(fullResolution.x * cropWindow.max.x)),
                      static_cast<int>(std::ceil(fullResolution.y * cropWindow.max.y)))))
        // clang-format on
        , pixels(new Pixel[croppedPixelBounds.area()])
        , scale(scale)
        , maxSampleLuminance(maxSampleLuminance) {

        /*TODO: log resolution, crop window and cropped bounds */

        const Float invTableExt = 1.f / static_cast<Float>(FILTER_TABLE_EXTENT);
        std::size_t offset = 0;
        for (int y = 0; y < FILTER_TABLE_EXTENT; ++y) {
            for (int x = 0; x < FILTER_TABLE_EXTENT; ++x) {
                // clang-format off
                Point2f p;
                p.x = (static_cast<Float>(x) + 0.5f) * invTableExt * filter->radius.x;
                p.y = (static_cast<Float>(y) + 0.5f) * invTableExt * filter->radius.y;
                // clang-format on

                filterTable[offset++] = filter->eval(p);
            }
        }
    }

    Film::Pixel& Film::getPixel(const Point2i& p) {
        assert(insideExclusive(p, croppedPixelBounds));

        const int width = croppedPixelBounds.max.x - croppedPixelBounds.min.x;
        const int offset = (p.x - croppedPixelBounds.min.x) +
                           (p.y - croppedPixelBounds.min.y) * width;

        return pixels[offset];
    }

    Bounds2i Film::getSampleBounds() const {
        Point2i min(math::floor(Point2f(croppedPixelBounds.min) +
                                Vector2f(0.5f, 0.5f) - filter->radius));
        Point2i max(math::ceil(Point2f(croppedPixelBounds.max) -
                               Vector2f(0.5f, 0.5f) + filter->radius));
        Bounds2i bounds;
        bounds.min = min;
        bounds.max = max;

        return bounds;
    }

    Bounds2f Film::getPhysicalExtent() const {
        const Float aspect = static_cast<Float>(fullResolution.y) /
                             static_cast<Float>(fullResolution.x);
        const Float x =
            std::sqrt(diagonal * diagonal / (1.f + aspect * aspect));
        const Float y = aspect * x;

        return Bounds2f{Point2f(-x / 2.f, -y / 2.f), Point2f(x / 2.f, y / 2.f)};
    }
} // namespace idragnev::pbrt