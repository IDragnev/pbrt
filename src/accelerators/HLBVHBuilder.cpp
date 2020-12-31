#include "pbrt/accelerators/BVHBuilders.hpp"
#include "pbrt/functional/Functional.hpp"
#include "pbrt/parallel/Parallel.hpp"

#include <numeric>

namespace idragnev::pbrt::accelerators::bvh {
    namespace constants {
        constexpr std::uint32_t MORTON_CODE_BITS = 30;
        constexpr std::uint32_t MORTON_DIMENSION_BITS = MORTON_CODE_BITS / 3;
        constexpr std::uint32_t MORTON_DIMENSION_MAX = 1 << MORTON_DIMENSION_BITS;
    } // namespace constants

    struct MortonPrimitive
    {
        std::size_t index = 0;
        std::uint32_t mortonCode = 0;
    };

    std::vector<MortonPrimitive>
    toMortonPrimitives(const std::vector<PrimitiveInfo>& primsInfo,
                       const Bounds3f& centroidBounds);
    std::uint32_t encodeMorton3(const Vector3f& v);
    std::uint32_t leftShift3(std::uint32_t x);

    [[nodiscard]] std::vector<MortonPrimitive>
    radixSort(std::vector<MortonPrimitive> vec);

    BuildResult HLBVHBuilder::operator()(const PrimsVec& primitives) const {
        BuildResult result{};
        if (primitives.empty()) {
            return result;
        }

        const auto primsInfo = functional::fmapIndexed(
            primitives,
            [](const auto& primitive, const std::size_t i) {
                return PrimitiveInfo{i, primitive->worldBound()};
            });
        const Bounds3f centroidBounds = std::accumulate(
            primsInfo.cbegin(),
            primsInfo.cend(),
            Bounds3f{},
            [](const Bounds3f& acc, const PrimitiveInfo& info) {
                return unionOf(acc, info.centroid);
            });

        const std::vector<MortonPrimitive> mortonPrims =
            radixSort(toMortonPrimitives(primsInfo, centroidBounds));

        //...

        return result;
    }

    std::vector<MortonPrimitive>
    toMortonPrimitives(const std::vector<PrimitiveInfo>& primsInfo,
                       const Bounds3f& centroidBounds) {
        std::vector<MortonPrimitive> result{primsInfo.size()};

        constexpr std::int64_t CHUNK_SIZE = 512;
        const auto iterationsCount =
            static_cast<std::int64_t>(primsInfo.size());

        parallel::parallelFor(
            [&result, &primsInfo, &centroidBounds](const std::int64_t i) {
                const auto& info = primsInfo[static_cast<std::size_t>(i)];
                auto& primitive = result[static_cast<std::size_t>(i)];

                const Vector3f centroidOffset =
                    centroidBounds.offset(info.centroid);
                primitive.index = info.index;
                primitive.mortonCode = encodeMorton3(
                    constants::MORTON_DIMENSION_MAX * centroidOffset);
            },
            iterationsCount,
            CHUNK_SIZE);

        return result;
    }

    std::uint32_t encodeMorton3(const Vector3f& v) {
        const auto x = static_cast<std::uint32_t>(v.x);
        const auto y = static_cast<std::uint32_t>(v.y);
        const auto z = static_cast<std::uint32_t>(v.z);

        assert(x >= 0 && y >= 0 && z >= 0);

        return (leftShift3(z) << 2) |
               (leftShift3(y) << 1) |
                leftShift3(x);
    }

    std::uint32_t leftShift3(std::uint32_t x) {
        using constants::MORTON_DIMENSION_MAX;

        assert(x <= MORTON_DIMENSION_MAX);
        if (x == MORTON_DIMENSION_MAX) {
            --x;
        }

        x = (x | (x << 16)) & 0b00000011000000000000000011111111;
        // x = ---- --98 ---- ---- ---- ---- 7654 3210

        x = (x | (x << 8)) & 0b00000011000000001111000000001111;
        // x = ---- --98 ---- ---- 7654 ---- ---- 3210

        x = (x | (x << 4)) & 0b00000011000011000011000011000011;
        // x = ---- --98 ---- 76-- --54 ---- 32-- --10

        x = (x | (x << 2)) & 0b00001001001001001001001001001001;
        // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0

        return x;
    }

    std::vector<MortonPrimitive> radixSort(std::vector<MortonPrimitive> input) {
        using constants::MORTON_CODE_BITS;

        constexpr std::size_t BITS_PER_PASS = 6;
        static_assert((MORTON_CODE_BITS % BITS_PER_PASS) == 0,
                      "BITS_PER_PASS must evenly divide MORTON_CODE_BITS");

        constexpr std::size_t PASSES_COUNT = MORTON_CODE_BITS / BITS_PER_PASS;
        constexpr std::size_t BUCKETS_COUNT = 1 << BITS_PER_PASS;

        const auto bucketIndex = [](const std::uint32_t mortonCode,
                                    const std::size_t lowBit) {
            constexpr std::size_t mask = (1 << BITS_PER_PASS) - 1;
            const auto result = (mortonCode >> lowBit) & mask;

            assert(result >= 0);
            assert(result < BUCKETS_COUNT);

            return result;
        };

        std::vector<MortonPrimitive> temp{input.size()};
        for (std::size_t pass = 0; pass < PASSES_COUNT; ++pass) {
            const std::size_t lowBit = pass * BITS_PER_PASS;

            const std::vector<MortonPrimitive>& in = (pass & 1) ? temp : input;
            std::vector<MortonPrimitive>& out = (pass & 1) ? input : temp;

            std::size_t bucketSizes[BUCKETS_COUNT] = {};
            for (const MortonPrimitive& mp : in) {
                const std::size_t index = bucketIndex(mp.mortonCode, lowBit);
                ++bucketSizes[index];
            }

            std::size_t outIndices[BUCKETS_COUNT] = {};
            for (std::size_t i = 1; i < BUCKETS_COUNT; ++i) {
                outIndices[i] = outIndices[i - 1] + bucketSizes[i - 1];
            }

            for (const MortonPrimitive& mp : in) {
                const std::size_t bucket = bucketIndex(mp.mortonCode, lowBit);
                const std::size_t outIndex = outIndices[bucket];

                out[outIndex] = mp;
                ++outIndices[bucket];
            }
        }

        if constexpr (PASSES_COUNT & 1) {
            return temp;
        }
        else {
            return input;
        }
    }
} // namespace idragnev::pbrt::accelerators::bvh