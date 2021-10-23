#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/geometry/Utility.hpp"
#include "pbrt/core/sampling/Filter.hpp"
#include "pbrt/core/AtomicFloat.hpp"
#include "pbrt/core/math/Point2.hpp"
#include "pbrt/core/geometry/Bounds2.hpp"

#include <memory>
#include <string>
#include <mutex>

namespace idragnev::pbrt {
    class Film
    {
    public:
        Film(const Point2i& resolution,
             const Bounds2f& cropWindow,
             std::unique_ptr<const Filter> filter,
             Float diagonal,
             const std::string& filename,
             Float scale,
             Float maxSampleLuminance = constants::Infinity);

        Film(const Film&) = delete;
        Film& operator=(const Film&) = delete;

        Bounds2i getSampleBounds() const;
        Bounds2f getPhysicalExtent() const;

        // std::unique_ptr<FilmTile> getFilmTile(const Bounds2i& sampleBounds);
        // void mergeFilmTile(std::unique_ptr<FilmTile> tile);
        // void setImage(const Spectrum* img) const;
        // void addSplat(const Point2f& p, Spectrum v);
        // void writeImage(Float splatScale = 1.f);
        // void clear();

        const Point2i fullResolution;
        const Float diagonal;
        const std::unique_ptr<const Filter> filter;
        const std::string filename = "";
        const Bounds2i croppedPixelBounds;

    private:
        struct Pixel
        {
            Float xyz[3] = {0.f, 0.f, 0.f};
            Float filterWeightSum = 0.f;
            AtomicFloat splatXYZ[3];
            Float padding = 0.f;
        };

        Pixel& getPixel(const Point2i& p);

    private:
        std::unique_ptr<Pixel[]> pixels = nullptr;
        static constexpr int FILTER_TABLE_EXTENT = 16;
        Float filterTable[FILTER_TABLE_EXTENT * FILTER_TABLE_EXTENT];
        std::mutex mutex;

        [[maybe_unused]] const Float scale;
        [[maybe_unused]] const Float maxSampleLuminance;
    };
} // namespace idragnev::pbrt