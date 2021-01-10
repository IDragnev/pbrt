#include "pbrt/accelerators/bvh/HLBVHBuilder.hpp"
#include "pbrt/functional/Functional.hpp"
#include "pbrt/parallel/Parallel.hpp"

#include <numeric>

namespace idragnev::pbrt::accelerators::bvh {
    namespace constants {
        constexpr std::uint32_t MORTON_CODE_BITS = 30;
        constexpr std::uint32_t MORTON_DIMENSION_BITS = MORTON_CODE_BITS / 3;
        constexpr std::uint32_t MORTON_DIMENSION_MAX = 1 << MORTON_DIMENSION_BITS;
        constexpr std::uint32_t MORTON_CODE_CLUSTER_BITS = 12;
        constexpr std::uint32_t MORTON_CODE_CLUSTER_MASK = 0b00111111111111000000000000000000;
    } // namespace constants

    struct MortonPrimitive
    {
        std::size_t index = 0;
        std::uint32_t mortonCode = 0;
    };

    std::vector<MortonPrimitive>
    toMortonPrimitives(const std::vector<PrimitiveInfo>& primsInfo);
    std::uint32_t encodeMorton3(const Vector3f& v);
    std::uint32_t leftShift3(std::uint32_t x);

    [[nodiscard]] std::vector<MortonPrimitive>
    radixSort(std::vector<MortonPrimitive> vec);

    Optional<std::size_t>
    findSplitPosition(const std::span<const MortonPrimitive> prims,
                      const std::uint32_t splitMask) noexcept;

    // Represents a cluster of primitives which
    // have matching bits for the selected morton code mask.
    // Each cluster is placed in the contiguous range
    // [firstPrimitiveIndex .. firstPrimitiveIndex + primitivesCount)
    // of the primitives array when building the BVH.
    // The `nodes` field holds the allocated nodes for the cluster
    // which are used to build the LBVH for this cluster.
    // Treelets are used to build the lower levels of the BVH.
    struct LBVHTreelet
    {
        std::size_t firstPrimitiveIndex = 0;
        std::size_t primitivesCount = 0;
        BuildNode* nodes = nullptr;
    };

    std::vector<LBVHTreelet>
    splitToTreelets(const std::vector<MortonPrimitive>& prims,
                    memory::MemoryArena& arena,
                    const bool zeroInitializeAllocatedNodes);

    BuildResult HLBVHBuilder::operator()(memory::MemoryArena& arena,
                                         const PrimsVec& primitives) { 
        if (primitives.empty()) {
            return BuildResult{};
        }

        [[maybe_unused]] const auto cleanUp =
            functional::ScopedFn{[&b = *this]() noexcept {
                b = {};
            }};
        
        this->prims = &primitives;
        this->primitivesInfo = functional::fmapIndexed(
            primitives,
            [](const auto& primitive, const std::size_t i) {
                return PrimitiveInfo{i, primitive->worldBound()};
            });
        this->mortonPrimInfos =
            radixSort(toMortonPrimitives(this->primitivesInfo));

        std::vector<LBVHTreelet> treelets =
            splitToTreelets(this->mortonPrimInfos, arena, false);
        LowerLevels lls = buildLowerLevels(std::move(treelets));

        return buildBVH(arena, std::move(lls));
    }

    std::vector<MortonPrimitive>
    toMortonPrimitives(const std::vector<PrimitiveInfo>& primsInfo) {
        std::vector<MortonPrimitive> result{primsInfo.size()};

        const Bounds3f centroidBounds =
            std::accumulate(primsInfo.cbegin(),
                            primsInfo.cend(),
                            Bounds3f{},
                            [](const Bounds3f& acc, const PrimitiveInfo& info) {
                                return unionOf(acc, info.centroid);
                            });

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

    // Generates a vector of `LBVHTreelet`s -
    // one treelet for each cluster of primitives with matching
    // morton code bits for the selected bit mask.
    // Conditionally zero-initializes the nodes allocated with `arena`.
    //
    // (!) Assumes  that `mortonPrims` are sorted by
    // MortonPrimitive::mortonCode. (!)
    std::vector<LBVHTreelet>
    splitToTreelets(const std::vector<MortonPrimitive>& mortonPrims,
                    memory::MemoryArena& arena,
                    const bool initializeAllocatedNodes) {
        using constants::MORTON_CODE_CLUSTER_MASK;

        std::vector<LBVHTreelet> result;

        for (std::size_t first = 0, last = 1; last <= mortonPrims.size();
             ++last) {
            const bool isLastCluster = last == mortonPrims.size();
            const bool isClusterBorder =
                (isLastCluster == false) &&
                ((mortonPrims[first].mortonCode & MORTON_CODE_CLUSTER_MASK) !=
                 (mortonPrims[last].mortonCode & MORTON_CODE_CLUSTER_MASK));

            if (isLastCluster || isClusterBorder) {
                const std::size_t clusterSize = last - first;
                const std::size_t maxBVHNodes = 2 * clusterSize;

                result.push_back(LBVHTreelet{
                    .firstPrimitiveIndex = first,
                    .primitivesCount = clusterSize,
                    .nodes = arena.alloc<BuildNode>(maxBVHNodes,
                                                    initializeAllocatedNodes),
                });

                first = last;
            }
        }

        return result;
    }

    HLBVHBuilder::LowerLevels
    HLBVHBuilder::buildLowerLevels(std::vector<LBVHTreelet>&& treelets) const {
        // start with the highest morton code bit which is not guaranteed
        // to be the same for each primitive in the cluster
        constexpr std::size_t splitBit = (constants::MORTON_CODE_BITS - 1) -
                                         constants::MORTON_CODE_CLUSTER_BITS;

        std::atomic<std::size_t> nodesCount = 0;
        std::atomic<std::size_t> orderedPrimsFreePosition = 0;
        PrimsVec orderedPrimitives = {this->prims->size(), nullptr};

        parallel::parallelFor(
            [&treelets,
             &nodesCount,
             &orderedPrimsFreePosition,
             &orderedPrimitives,
             this](const std::int64_t i) {
                LBVHTreelet& treelet = treelets[static_cast<std::size_t>(i)];

                const auto lbvh = emitLBVH(
                    treelet.nodes,
                    std::span<const MortonPrimitive>{
                        &this->mortonPrimInfos[treelet.firstPrimitiveIndex],
                        treelet.primitivesCount},
                    orderedPrimitives,
                    orderedPrimsFreePosition,
                    splitBit);

                treelet.nodes = lbvh.root;
                nodesCount += lbvh.nodesCount;
            },
            treelets.size());

        return LowerLevels{
            .roots =
                functional::fmap(treelets, [](auto& t) { return t.nodes; }),
            .nodesCount = nodesCount,
            .orderedPrimitives = std::move(orderedPrimitives),
        };
    }

    // Recursively builds a BVH on the allocated `buildNodes`
    // using the range of morton primitives `primsRange`.
    // Writes the corresponding primitives in the `orderedPrims` subrange
    // [orderedPrimsFreePos .. orderedPrimsFreePos + primsRange.size)
    // Splits the primitives based on the `splitBit` of their morton code.
    // `buildNodes` is advanced each time a node is used,
    // leaving the pointer pointing to an unused node.
    //
    // (!) Assumes `primsRange` is sorted on MortonPrimitive::mortonCode (!)
    BuildTree
    HLBVHBuilder::emitLBVH(BuildNode*& buildNodes,
                           const std::span<const MortonPrimitive> primsRange,
                           PrimsVec& orderedPrims,
                           std::atomic<std::size_t>& orderedPrimsFreePos,
                           const int splitBit) const {
        if (splitBit < 0 || primsRange.size() < this->maxPrimitivesInNode) {
            const std::size_t firstPrimIndex =
                orderedPrimsFreePos.fetch_add(primsRange.size());
            BuildNode* const node = buildNodes++;
            *node = makeLeafNode(primsRange, firstPrimIndex, orderedPrims);

            return BuildTree{
                .root = node,
                .nodesCount = 1,
            };
        }
        else {
            const std::uint32_t splitMask = 1 << splitBit;
            if ((primsRange.front().mortonCode & splitMask) ==
                (primsRange.back().mortonCode & splitMask))
            {
                // prims are sorted -> they all match in this bit
                return emitLBVH(buildNodes,
                                primsRange,
                                orderedPrims,
                                orderedPrimsFreePos,
                                splitBit - 1);
            }

            const auto splitPos = findSplitPosition(primsRange, splitMask)
                                      .or_else([] {
                                          assert(false);
                                          return 0;
                                      })
                                      .value();

            BuildNode* const node = buildNodes++;
            const auto left = emitLBVH(buildNodes,
                                       primsRange.first(splitPos),
                                       orderedPrims,
                                       orderedPrimsFreePos,
                                       splitBit - 1);
            const auto right = emitLBVH(buildNodes,
                                        primsRange.subspan(splitPos),
                                        orderedPrims,
                                        orderedPrimsFreePos,
                                        splitBit - 1);
            const std::size_t splitAxis = splitBit % 3;
            *node = BuildNode::Interior(splitAxis, left.root, right.root);

            return BuildTree{
                .root = node,
                .nodesCount = left.nodesCount + right.nodesCount + 1,
            };
        }
    }

    BuildNode HLBVHBuilder::makeLeafNode(
        const std::span<const MortonPrimitive> primsRange,
        const std::size_t firstPrimIndex,
        PrimsVec& orderedPrims) const {
        const PrimsVec& primitives = *(this->prims);

        Bounds3f bounds;
        for (std::size_t writePos = firstPrimIndex;
             const MortonPrimitive& mp : primsRange)
        {
            bounds = unionOf(bounds, this->primitivesInfo[mp.index].bounds);

            orderedPrims[writePos] = primitives[mp.index];
            ++writePos;
        }

        return BuildNode::Leaf(firstPrimIndex, primsRange.size(), bounds);
    }

    // Finds the position of the first primitive in `prims`
    // which differs from its predecessor in the `splitMask` bits
    // of its morton code.
    //
    // (!) Assumes that `prims` is sorted by MortonPrimitive::mortonCode. (!)
    Optional<std::size_t>
    findSplitPosition(const std::span<const MortonPrimitive> prims,
                      const std::uint32_t splitMask) noexcept {
        if (prims.empty()) {
            return pbrt::nullopt;
        }

        std::size_t left = 0;
        std::size_t right = prims.size() - 1;

        while (left + 1 != right) {
            const auto mid = (left + right) / 2;

            if ((prims[left].mortonCode & splitMask) ==
                (prims[mid].mortonCode & splitMask)) {
                // since the prims are sorted, this indicates
                // that prims[left .. mid] match in those bits
                left = mid;
            }
            else {
                right = mid;
            }
        }

        if ((prims[left].mortonCode & splitMask) !=
            (prims[right].mortonCode & splitMask))
        {
            return pbrt::make_optional(right);
        }
        else {
            return pbrt::nullopt;
        }
    }
} // namespace idragnev::pbrt::accelerators::bvh