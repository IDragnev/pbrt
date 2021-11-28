#include "pbrt/core/Film.hpp"
#include "pbrt/core/geometry/Utility.hpp"
#include "pbrt/memory/Memory.hpp"

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

    std::unique_ptr<FilmTile> Film::getFilmTile(const Bounds2i& sampleBounds) {
        const Bounds2f floatBounds{sampleBounds};
        const Point2i p0{
            math::ceil(toDiscrete(floatBounds.min) - filter->radius)};
        const Point2i p1 =
            Point2i{math::floor(toDiscrete(floatBounds.max) + filter->radius)} +
            Vector2i(1, 1);

        Bounds2i tilePixelBounds =
            intersectionOf(Bounds2i(p0, p1), croppedPixelBounds);

        return std::unique_ptr<FilmTile>(new FilmTile(
            tilePixelBounds,
            filter->radius,
            std::span<const Float>(filterTable,
                                   FILTER_TABLE_EXTENT * FILTER_TABLE_EXTENT),
            FILTER_TABLE_EXTENT,
            maxSampleLuminance));
    }

    void Film::mergeFilmTile(std::unique_ptr<FilmTile> tile) {
        // TODO: Log tile->pixelBounds ...

        std::lock_guard<std::mutex> lock(mutex);

        for (const Point2i pixel : tile->getPixelBounds()) {
            const FilmTilePixel& tilePixel = tile->getPixel(pixel);
            const std::array<Float, 3> xyz = tilePixel.contribSum.toXYZ();

            Pixel& filmPixel = this->getPixel(pixel);
            for (std::size_t i = 0; i < 3; ++i) {
                filmPixel.xyz[i] += xyz[i];
            }
            filmPixel.filterWeightSum += tilePixel.filterWeightSum;
        }
    }

    void Film::setImage(const std::span<Spectrum> imagePixels) const {
        if (const int filmPixels = croppedPixelBounds.area();
            filmPixels == static_cast<int>(imagePixels.size()))
        {
            const std::size_t nPixels = imagePixels.size();
            for (std::size_t i = 0; i < nPixels; ++i) {
                const std::array xyz = imagePixels[i].toXYZ();

                Pixel& filmPixel = this->pixels[i];

                filmPixel.xyz[0] = xyz[0];
                filmPixel.xyz[1] = xyz[1];
                filmPixel.xyz[2] = xyz[2];

                filmPixel.filterWeightSum = 1.f;

                filmPixel.splatXYZ[0] = 0.f;
                filmPixel.splatXYZ[1] = 0.f;
                filmPixel.splatXYZ[2] = 0.f;
            }
        }
        else {
            // TODO : Log ...
            assert(false);
        }
    }

    void Film::clear() {
        for (const Point2i p : croppedPixelBounds) {
            Pixel& filmPixel = getPixel(p);

            filmPixel.xyz[0] = 0.f;
            filmPixel.xyz[1] = 0.f;
            filmPixel.xyz[2] = 0.f;

            filmPixel.filterWeightSum = 0.f;

            filmPixel.splatXYZ[0] = 0.f;
            filmPixel.splatXYZ[1] = 0.f;
            filmPixel.splatXYZ[2] = 0.f;
        }
    }

    void
    FilmTile::addSample(Point2f pFilm, Spectrum L, const Float sampleWeight) {
        if (L.y() > maxSampleLuminance) {
            L *= maxSampleLuminance / L.y();
        }

        pFilm = toDiscrete(pFilm);

        auto p0 = Point2i{math::ceil(pFilm - filterRadius)};
        auto p1 = Point2i{math::floor(pFilm + filterRadius)} + Vector2i(1, 1);
        p0 = math::max(p0, pixelBounds.min);
        p1 = math::min(p1, pixelBounds.max);

        std::size_t* const filterXOffsets =
            PBRT_ALLOCA(std::size_t, std::max(p1.x - p0.x, 1));
        for (int x = p0.x; x < p1.x; ++x) {
            const Float fx = std::abs((x - pFilm.x) * invFilterRadius.x *
                                      static_cast<Float>(filterTableExtent));

            filterXOffsets[x - p0.x] =
                std::min(static_cast<std::size_t>(std::floor(fx)),
                         filterTableExtent - 1);
        }

        std::size_t* const filterYOffsets =
            PBRT_ALLOCA(std::size_t, std::max(p1.y - p0.y, 1));
        for (int y = p0.y; y < p1.y; ++y) {
            const Float fy = std::abs((y - pFilm.y) * invFilterRadius.y *
                                      static_cast<Float>(filterTableExtent));
            filterYOffsets[y - p0.y] =
                std::min(static_cast<std::size_t>(std::floor(fy)),
                         filterTableExtent - 1);
        }

        for (int y = p0.y; y < p1.y; ++y) {
            for (int x = p0.x; x < p1.x; ++x) {
                std::size_t offset =
                    filterYOffsets[y - p0.y] * filterTableExtent +
                    filterXOffsets[x - p0.x];
                const Float filterWeight = filterTable[offset];

                FilmTilePixel& pixel = getPixel(Point2i(x, y));
                pixel.contribSum += L * sampleWeight * filterWeight;
                pixel.filterWeightSum += filterWeight;
            }
        }
    }

    void Film::addSplat(const Point2f& p, Spectrum v) {
        if (v.hasNaNs()) {
            // TODO: log nans
            return;
        }
        else if (v.y() < Float(0)) {
            // log negative luminace
            return;
        }
        else if (std::isinf(v.y())) {
            // log inf luminace
            return;
        }

        const Point2i pi(math::floor(p));
        if (insideExclusive(pi, croppedPixelBounds)) {
            if (v.y() > maxSampleLuminance) {
                v *= maxSampleLuminance / v.y();
            }

            const std::array xyz = v.toXYZ();

            Pixel& pixel = getPixel(pi);
            pixel.splatXYZ[0].add(xyz[0]);
            pixel.splatXYZ[1].add(xyz[1]);
            pixel.splatXYZ[2].add(xyz[2]);
        }
    }

    void Film::writeImage(const Float splatScale) {
        // TODO: Log info: "Converting image to RGB and computing final weighted pixel values..."

        const int nPixels = croppedPixelBounds.area();
        std::unique_ptr<Float[]> imageRGB(new Float[3 * nPixels]);

        for (std::size_t pixelIndex = 0; const Point2i p : croppedPixelBounds) {
            const std::span<Float, 3> pixelRGB{&(imageRGB[3 * pixelIndex]), 3};

            Pixel& pixel = getPixel(p);

            const std::array arrRGB =
                XYZToRGB(std::span<const Float, 3>{pixel.xyz});
            pixelRGB[0] = arrRGB[0];
            pixelRGB[1] = arrRGB[1];
            pixelRGB[2] = arrRGB[2];

            // normalize pixel with weight sum
            if (pixel.filterWeightSum != 0.f) {
                const Float invWt = Float(1) / pixel.filterWeightSum;

                pixelRGB[0] = std::max(Float(0), pixelRGB[0] * invWt);
                pixelRGB[1] = std::max(Float(0), pixelRGB[1] * invWt);
                pixelRGB[2] = std::max(Float(0), pixelRGB[2] * invWt);
            }

            const Float splatXYZ[3] = {pixel.splatXYZ[0],
                                       pixel.splatXYZ[1],
                                       pixel.splatXYZ[2]};
            const std::array splatRGB = XYZToRGB(splatXYZ);

            pixelRGB[0] += splatScale * splatRGB[0];
            pixelRGB[1] += splatScale * splatRGB[1];
            pixelRGB[2] += splatScale * splatRGB[2];

            pixelRGB[0] *= scale;
            pixelRGB[1] *= scale;
            pixelRGB[2] *= scale;

            ++pixelIndex;
        }

        // TODO: log_info("Writing image to {}. bounds = {}, scale = {}, splatScale = {}",
        //                filename, croppedPixelBounds, scale, splatScale)
        // pbrt::writeImage(filename, &imageRGB[0], croppedPixelBounds, fullResolution);
    }

    FilmTilePixel& FilmTile::getPixel(const Point2i& p) {
        assert(insideExclusive(p, pixelBounds));

        int width = pixelBounds.max.x - pixelBounds.min.x;
        int offset = (p.y - pixelBounds.min.y) * width +
                     (p.x - pixelBounds.min.x);

        return pixels[offset];
    }

    const FilmTilePixel& FilmTile::getPixel(const Point2i& p) const {
        assert(insideExclusive(p, pixelBounds));

        int width = pixelBounds.max.x - pixelBounds.min.x;
        int offset = (p.y - pixelBounds.min.y) * width +
                     (p.x - pixelBounds.min.x);

        return pixels[offset];
    }
} // namespace idragnev::pbrt