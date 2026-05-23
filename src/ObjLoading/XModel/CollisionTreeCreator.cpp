#include "CollisionTreeCreator.h"

#include "Utils/Logging/Log.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace
{
    struct GenericAabbTree
    {
        int firstItem;
        int itemCount;
        int firstChild;
        int childCount;
    };

    struct GenericAabbTreeOptions
    {
        void* items;
        int itemCount;
        int itemSize;
        int maintainValidBounds;
        float (*mins)[3];
        float (*maxs)[3];
        GenericAabbTree* treeNodePool;
        int treeNodeLimit;
        int minItemsPerLeaf;
        int maxItemsPerLeaf;
    };

    class Bounds
    {
    public:
        Bounds()
            : m_mins(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
              m_maxs(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max())
        {
        }

        void ExpandFromTri(const std::array<Eigen::Vector3f, 3>& triCoordinates)
        {
            for (const auto& vert : triCoordinates)
            {
                m_mins[0] = std::min(m_mins[0], vert.x());
                m_mins[1] = std::min(m_mins[1], vert.y());
                m_mins[2] = std::min(m_mins[2], vert.z());
                m_maxs[0] = std::max(m_maxs[0], vert.x());
                m_maxs[1] = std::max(m_maxs[1], vert.y());
                m_maxs[2] = std::max(m_maxs[2], vert.z());
            }
        }

        void ExpandFromBounds(const Bounds& otherBounds)
        {
            m_mins[0] = std::min(m_mins[0], otherBounds.m_mins[0]);
            m_mins[1] = std::min(m_mins[1], otherBounds.m_mins[1]);
            m_mins[2] = std::min(m_mins[2], otherBounds.m_mins[2]);
            m_maxs[0] = std::max(m_maxs[0], otherBounds.m_maxs[0]);
            m_maxs[1] = std::max(m_maxs[1], otherBounds.m_maxs[1]);
            m_maxs[2] = std::max(m_maxs[2], otherBounds.m_maxs[2]);
        }

        void ExpandFromBounds(const float (&otherMins)[3], const float (&otherMaxs)[3])
        {
            m_mins[0] = std::min(m_mins[0], otherMins[0]);
            m_mins[1] = std::min(m_mins[1], otherMins[1]);
            m_mins[2] = std::min(m_mins[2], otherMins[2]);
            m_maxs[0] = std::max(m_maxs[0], otherMaxs[0]);
            m_maxs[1] = std::max(m_maxs[1], otherMaxs[1]);
            m_maxs[2] = std::max(m_maxs[2], otherMaxs[2]);
        }

        [[nodiscard]] Eigen::Vector3f GetDelta() const
        {
            return m_maxs - m_mins;
        }

        [[nodiscard]] float GetVolume() const
        {
            return GetDelta().prod();
        }

        Eigen::Vector3f m_mins;
        Eigen::Vector3f m_maxs;
    };

    void ClearBounds(float* mins, float* maxs)
    {
        mins[0] = FLT_MAX;
        mins[1] = FLT_MAX;
        mins[2] = FLT_MAX;
        maxs[0] = -FLT_MAX;
        maxs[1] = -FLT_MAX;
        maxs[2] = -FLT_MAX;
    }

    void ExpandBounds(const float* addedmins, const float* addedmaxs, float* mins, float* maxs)
    {
        *mins = std::min(*mins, *addedmins);
        *maxs = std::max(*addedmaxs, *maxs);
        mins[1] = std::min(mins[1], addedmins[1]);
        maxs[1] = std::max(addedmaxs[1], maxs[1]);
        mins[2] = std::min(mins[2], addedmins[2]);
        maxs[2] = std::max(addedmaxs[2], maxs[2]);
    }

    float AddedVolume(const float* addedmins, const float* addedmaxs, const float* mins, const float* maxs)
    {
        float expandedMins;   // [esp+0h] [ebp-20h] BYREF
        float v6;             // [esp+4h] [ebp-1Ch]
        float v7;             // [esp+8h] [ebp-18h]
        float expandedVolume; // [esp+Ch] [ebp-14h]
        float expandedMaxs;   // [esp+14h] [ebp-Ch] BYREF
        float v10;            // [esp+18h] [ebp-8h]
        float v11;            // [esp+1Ch] [ebp-4h]

        expandedMins = *mins;
        v6 = mins[1];
        v7 = mins[2];
        expandedMaxs = *maxs;
        v10 = maxs[1];
        v11 = maxs[2];
        ExpandBounds(addedmins, addedmaxs, &expandedMins, &expandedMaxs);
        expandedVolume = (expandedMaxs - expandedMins) * (v10 - v6) * (v11 - v7);

        return expandedVolume - (*maxs - *mins) * (maxs[1] - mins[1]) * (maxs[2] - mins[2]);
    }

    int compare_floats(const void* e0, const void* e1)
    {
        float diff; // [esp+4h] [ebp-4h]

        diff = *static_cast<const float*>(e0) - *static_cast<const float*>(e1);
        if (diff >= 0.0)
            return diff > 0.0;
        else
            return -1;
    }

    class AabbTreeBuilder
    {
    public:
        AabbTreeBuilder()
            : sortedMins(nullptr),
              sortedMaxs(nullptr),
              sortedCoplanar(nullptr),
              aabbTreeCount(0)
        {
        }

        int BuildAabbTree(GenericAabbTreeOptions* options)
        {
            static constexpr size_t STACK_BUFFER_SIZE = 64;

            float* v2;                                // [esp+4h] [ebp-454h]
            float* v3;                                // [esp+8h] [ebp-450h]
            float* v4;                                // [esp+Ch] [ebp-44Ch]
            float* v5;                                // [esp+10h] [ebp-448h]
            float (*boundCopies)[3];                  // [esp+44h] [ebp-414h]
            int* remap;                               // [esp+48h] [ebp-410h]
            int itemIndex;                            // [esp+4Ch] [ebp-40Ch]
            int itemIndexa;                           // [esp+4Ch] [ebp-40Ch]
            int itemIndexb;                           // [esp+4Ch] [ebp-40Ch]
            int itemIndexc;                           // [esp+4Ch] [ebp-40Ch]
            int remapBuffer[STACK_BUFFER_SIZE];       // [esp+50h] [ebp-408h] BYREF
            float sortedBounds[3][STACK_BUFFER_SIZE]; // [esp+150h] [ebp-308h] BYREF
            char* itemCopies;                         // [esp+454h] [ebp-4h]

            if (options->itemCount > STACK_BUFFER_SIZE)
            {
                remap = (int*)operator new(4 * options->itemCount);
                sortedMins = (float*)operator new(4 * options->itemCount);
                sortedMaxs = (float*)operator new(4 * options->itemCount);
                sortedCoplanar = (float*)operator new(4 * options->itemCount);
            }
            else
            {
                remap = remapBuffer;
                sortedMins = sortedBounds[0];
                sortedMaxs = sortedBounds[1];
                sortedCoplanar = sortedBounds[2];
            }

            for (itemIndex = 0; itemIndex < options->itemCount; ++itemIndex)
                remap[itemIndex] = itemIndex;

            options->treeNodePool->firstItem = 0;
            options->treeNodePool->itemCount = options->itemCount;

            aabbTreeCount = 1;
            BuildAabbTree_r(options->treeNodePool, options, remap);

            itemCopies = (char*)operator new(options->itemSize * options->itemCount);
            memcpy(itemCopies, options->items, options->itemSize * options->itemCount);
            for (itemIndexa = 0; itemIndexa < options->itemCount; ++itemIndexa)
                memcpy((char*)options->items + options->itemSize * itemIndexa, &itemCopies[options->itemSize * remap[itemIndexa]], options->itemSize);
            operator delete(itemCopies);

            if (options->maintainValidBounds)
            {
                boundCopies =
                    (float (*)[3]) operator new(4 * ((3 * (unsigned __int64)(unsigned int)options->itemCount) >> 32 != 0 ? -1 : 3 * options->itemCount));
                memcpy(boundCopies, options->mins, 12 * options->itemCount);
                for (itemIndexb = 0; itemIndexb < options->itemCount; ++itemIndexb)
                {
                    v4 = options->mins[itemIndexb];
                    v5 = boundCopies[remap[itemIndexb]];
                    *v4 = *v5;
                    v4[1] = v5[1];
                    v4[2] = v5[2];
                }
                memcpy(boundCopies, options->maxs, 12 * options->itemCount);
                for (itemIndexc = 0; itemIndexc < options->itemCount; ++itemIndexc)
                {
                    v2 = options->maxs[itemIndexc];
                    v3 = boundCopies[remap[itemIndexc]];
                    *v2 = *v3;
                    v2[1] = v3[1];
                    v2[2] = v3[2];
                }
                operator delete(boundCopies);
            }
            if (remap != remapBuffer)
            {
                operator delete(remap);
                operator delete(sortedMins);
                operator delete(sortedMaxs);
                operator delete(sortedCoplanar);
            }

            return aabbTreeCount;
        }

    private:
        void BuildAabbTree_r(GenericAabbTree* tree, GenericAabbTreeOptions* options, int* remap)
        {
            int midStart;             // [esp+0h] [ebp-10h] BYREF
            int childIndex;           // [esp+4h] [ebp-Ch]
            int lastStart;            // [esp+8h] [ebp-8h] BYREF
            GenericAabbTree* subtree; // [esp+Ch] [ebp-4h]

            assert(tree->itemCount);

            tree->firstChild = aabbTreeCount;
            tree->childCount = 0;
            if (tree->itemCount > options->maxItemsPerLeaf && SplitAabbTree(tree->itemCount, options, remap, &midStart, &lastStart))
            {
                subtree = &options->treeNodePool[aabbTreeCount];

                assert(tree->firstChild == aabbTreeCount);

                CreateAabbSubTrees(tree, options, remap, 0, midStart);
                if (midStart < lastStart)
                    CreateAabbSubTrees(tree, options, remap, midStart, lastStart - midStart);
                CreateAabbSubTrees(tree, options, remap, lastStart, tree->itemCount - lastStart);
                tree->childCount = aabbTreeCount - tree->firstChild;
                for (childIndex = 0; childIndex < tree->childCount; ++childIndex)
                    BuildAabbTree_r(&subtree[childIndex], options, &remap[subtree[childIndex].firstItem - tree->firstItem]);
            }
        }

        int SplitAabbTree(int count, GenericAabbTreeOptions* options, int* remap, int* midStart, int* lastStart)
        {
            float v6;         // [esp+4h] [ebp-58h]
            float v7;         // [esp+8h] [ebp-54h]
            int top;          // [esp+Ch] [ebp-50h]
            float (*mins)[3]; // [esp+10h] [ebp-4Ch]
            int splitAxis;    // [esp+14h] [ebp-48h] BYREF
            int bot;          // [esp+18h] [ebp-44h]
            float bounds[3];  // [esp+1Ch] [ebp-40h] BYREF
            float v13[3];     // [esp+28h] [ebp-34h] BYREF
            float v14[3];     // [esp+34h] [ebp-28h] BYREF
            float v15[3];     // [esp+40h] [ebp-1Ch] BYREF
            float (*maxs)[3]; // [esp+4Ch] [ebp-10h]
            float splitDist;  // [esp+50h] [ebp-Ch] BYREF
            int swapCache;    // [esp+54h] [ebp-8h]
            int mid;          // [esp+58h] [ebp-4h]

            mins = options->mins;
            maxs = options->maxs;
            if (!PickAabbSplitPlane(mins, maxs, remap, count, &splitAxis, &splitDist))
                return 0;
            ClearBounds(bounds, v13);
            ClearBounds(v14, v15);
            bot = 0;
            top = count - 1;
            while (bot <= top)
            {
                while (bot <= top && splitDist >= maxs[remap[bot]][splitAxis] && splitDist > mins[remap[bot]][splitAxis])
                {
                    ExpandBounds(mins[remap[bot]], maxs[remap[bot]], bounds, v13);
                    ++bot;
                }
                while (bot <= top && mins[remap[top]][splitAxis] >= splitDist && maxs[remap[top]][splitAxis] > splitDist)
                {
                    ExpandBounds(mins[remap[top]], maxs[remap[top]], v14, v15);
                    --top;
                }
                if (bot > top)
                    break;
                if ((mins[remap[bot]][splitAxis] < splitDist || maxs[remap[bot]][splitAxis] <= splitDist)
                    && (splitDist < maxs[remap[top]][splitAxis] || splitDist <= mins[remap[top]][splitAxis]))
                {
                    for (mid = bot; mid < top; ++mid)
                    {
                        if (mins[remap[mid]][splitAxis] >= splitDist && maxs[remap[mid]][splitAxis] > splitDist)
                        {
                            swapCache = remap[mid];
                            remap[mid] = remap[top];
                            remap[top] = swapCache;
                            break;
                        }
                        if (splitDist >= maxs[remap[mid]][splitAxis] && splitDist > mins[remap[mid]][splitAxis])
                        {
                            swapCache = remap[mid];
                            remap[mid] = remap[bot];
                            remap[bot] = swapCache;
                            break;
                        }
                    }
                    if (mid == top)
                        break;
                }
                else
                {
                    swapCache = remap[bot];
                    remap[bot] = remap[top];
                    remap[top] = swapCache;
                }
            }
            if (bot <= top && (bot < options->minItemsPerLeaf || top - bot + 1 < options->minItemsPerLeaf || count - top - 1 < options->minItemsPerLeaf))
            {
                while (bot <= top)
                {
                    while (bot <= top)
                    {
                        v7 = AddedVolume(mins[remap[bot]], maxs[remap[bot]], bounds, v13);
                        if (AddedVolume(mins[remap[bot]], maxs[remap[bot]], v14, v15) < (double)v7)
                            break;
                        ExpandBounds(mins[remap[bot]], maxs[remap[bot]], bounds, v13);
                        ++bot;
                    }
                    while (bot <= top)
                    {
                        v6 = AddedVolume(mins[remap[top]], maxs[remap[top]], v14, v15);
                        if (AddedVolume(mins[remap[top]], maxs[remap[top]], bounds, v13) < (double)v6)
                            break;
                        ExpandBounds(mins[remap[top]], maxs[remap[top]], v14, v15);
                        --top;
                    }
                    if (bot >= top)
                    {
                        if (bot == top)
                        {
                            if (2 * bot >= count)
                                --top;
                            else
                                ++bot;
                        }
                    }
                    else
                    {
                        swapCache = remap[bot];
                        remap[bot] = remap[top];
                        remap[top] = swapCache;
                        ++bot;
                        --top;
                    }
                }
            }
            if (!bot || bot == count)
                return 0;
            *midStart = bot;
            *lastStart = top + 1;
            return 1;
        }

        bool PickAabbSplitPlane(float (*mins)[3], float (*maxs)[3], int* remap, int count, int* chosenAxis, float* chosenDist)
        {
            float v7;                 // [esp+4h] [ebp-A0h]
            int sideSplitCount;       // [esp+38h] [ebp-6Ch]
            float nextDist;           // [esp+3Ch] [ebp-68h]
            int prevMinCount;         // [esp+40h] [ebp-64h]
            int prevOnCount;          // [esp+44h] [ebp-60h]
            float dist;               // [esp+48h] [ebp-5Ch]
            signed int minMaxCount;   // [esp+4Ch] [ebp-58h]
            signed int coplanarCount; // [esp+50h] [ebp-54h]
            int axisIndex;            // [esp+54h] [ebp-50h]
            signed int bestHeuristic; // [esp+58h] [ebp-4Ch]
            int smallestAxis;         // [esp+5Ch] [ebp-48h]
            int maxIndex;             // [esp+60h] [ebp-44h]
            float globalMaxs[3];      // [esp+64h] [ebp-40h] BYREF
            int sideFrontCount;       // [esp+70h] [ebp-34h]
            int i;                    // [esp+74h] [ebp-30h]
            int sideOnCount;          // [esp+78h] [ebp-2Ch]
            float globalMins[3];      // [esp+7Ch] [ebp-28h] BYREF
            int axisBias[3];          // [esp+88h] [ebp-1Ch]
            int minIndex;             // [esp+94h] [ebp-10h]
            int onIndex;              // [esp+98h] [ebp-Ch]
            int heuristic;            // [esp+9Ch] [ebp-8h]
            int sideBackCount;        // [esp+A0h] [ebp-4h]

            ClearBounds(globalMins, globalMaxs);

            for (i = 0; i < count; ++i)
                ExpandBounds(mins[remap[i]], maxs[remap[i]], globalMins, globalMaxs);

            smallestAxis = globalMaxs[0] - globalMins[0] > globalMaxs[1] - globalMins[1];
            if (globalMaxs[smallestAxis] - globalMins[smallestAxis] > globalMaxs[2] - globalMins[2])
                smallestAxis = 2;

            for (i = 0; i < 3; ++i)
            {
                axisBias[i] = static_cast<int>((globalMaxs[i] - globalMins[i] + 1.0f) * 10.0f / (globalMaxs[smallestAxis] - globalMins[smallestAxis] + 1.0f)
                                               + 0.4999999990686774);
            }

            bestHeuristic = -1;

            for (axisIndex = 0; axisIndex < 3; ++axisIndex)
            {
                minMaxCount = 0;
                coplanarCount = 0;

                for (i = 0; i < count; ++i)
                {
                    if (mins[remap[i]][axisIndex] == maxs[remap[i]][axisIndex])
                    {
                        sortedCoplanar[coplanarCount++] = mins[remap[i]][axisIndex];
                    }
                    else
                    {
                        sortedMins[minMaxCount] = mins[remap[i]][axisIndex];
                        sortedMaxs[minMaxCount++] = maxs[remap[i]][axisIndex];
                    }
                }

                qsort(sortedMins, minMaxCount, 4u, compare_floats);
                qsort(sortedMaxs, minMaxCount, 4u, compare_floats);
                qsort(sortedCoplanar, coplanarCount, 4u, compare_floats);

                sideFrontCount = 0;
                sideBackCount = count;
                sideSplitCount = 0;
                sideOnCount = 0;
                minIndex = 0;
                maxIndex = 0;
                onIndex = 0;
                prevMinCount = 0;
                prevOnCount = 0;

                // if (*sortedCoplanar - *sortedMins < 0.0f)
                if (coplanarCount && *sortedCoplanar - *sortedMins < 0.0f)
                    v7 = *sortedCoplanar;
                else
                    v7 = *sortedMins;

                nextDist = v7;
                while (nextDist < FLT_MAX)
                {
                    dist = nextDist;
                    nextDist = FLT_MAX;
                    sideSplitCount += prevMinCount;
                    sideBackCount -= prevMinCount;
                    prevMinCount = 0;

                    while (minIndex < minMaxCount && sortedMins[minIndex] == dist)
                    {
                        ++prevMinCount;
                        ++minIndex;
                    }

                    if (minIndex < minMaxCount && sortedMins[minIndex] < nextDist)
                        nextDist = sortedMins[minIndex];

                    while (maxIndex < minMaxCount && sortedMaxs[maxIndex] == dist)
                    {
                        ++sideFrontCount;
                        --sideSplitCount;
                        ++maxIndex;
                    }

                    if (maxIndex < minMaxCount && nextDist > sortedMaxs[maxIndex])
                        nextDist = sortedMaxs[maxIndex];

                    sideFrontCount += prevOnCount;
                    sideOnCount -= prevOnCount;
                    prevOnCount = 0;

                    while (onIndex < coplanarCount && sortedCoplanar[onIndex] == dist)
                    {
                        ++prevOnCount;
                        ++onIndex;
                    }

                    sideOnCount += prevOnCount;
                    sideBackCount -= prevOnCount;

                    if (onIndex < coplanarCount && nextDist > sortedCoplanar[onIndex])
                        nextDist = sortedCoplanar[onIndex];

                    assert(sideFrontCount + sideBackCount + sideSplitCount + sideOnCount == count);
                    assert(sideFrontCount >= 0);
                    assert(sideBackCount >= 0);
                    assert(sideSplitCount >= 0);
                    assert(sideOnCount >= 0);

                    if (sideFrontCount > 1 && sideBackCount > 1)
                    {
                        heuristic = axisBias[axisIndex] + count - std::abs(sideFrontCount - sideBackCount) - sideOnCount - 4 * sideSplitCount;
                        if (!sideOnCount && !sideSplitCount && !prevMinCount)
                        {
                            // heuristic += (int)((float)(nextDist - dist) + 9.313225746154785e-10);
                            heuristic += static_cast<int>(nextDist - dist);
                        }

                        if (heuristic > bestHeuristic)
                        {
                            bestHeuristic = heuristic;
                            *chosenAxis = axisIndex;
                            if (sideOnCount || sideSplitCount || prevMinCount)
                                *chosenDist = dist;
                            else
                                *chosenDist = (dist + nextDist) * 0.5f;
                        }
                    }
                }
            }

            return bestHeuristic != -1;
        }

        void CreateAabbSubTrees(GenericAabbTree* tree, GenericAabbTreeOptions* options, int* remap, int firstIndex, int count)
        {
            int midStart;             // [esp+0h] [ebp-Ch] BYREF
            int lastStart;            // [esp+4h] [ebp-8h] BYREF
            GenericAabbTree* subtree; // [esp+8h] [ebp-4h]

            if (count > options->maxItemsPerLeaf && SplitAabbTree(count, options, &remap[firstIndex], &midStart, &lastStart))
            {
                subtree = AllocAabbTreeNode(options);
                subtree->firstItem = firstIndex + tree->firstItem;
                subtree->itemCount = midStart;
                if (midStart < lastStart)
                {
                    subtree = AllocAabbTreeNode(options);
                    subtree->firstItem = midStart + firstIndex + tree->firstItem;
                    subtree->itemCount = lastStart - midStart;
                }
                subtree = AllocAabbTreeNode(options);
                subtree->firstItem = lastStart + firstIndex + tree->firstItem;
                subtree->itemCount = count - lastStart;
            }
            else
            {
                subtree = AllocAabbTreeNode(options);
                subtree->firstItem = firstIndex + tree->firstItem;
                subtree->itemCount = count;
            }
        }

        GenericAabbTree* AllocAabbTreeNode(GenericAabbTreeOptions* options)
        {
            if (aabbTreeCount == options->treeNodeLimit)
            {
                con::error("More than {} AABB nodes needed", options->treeNodeLimit);
                throw std::runtime_error("More than {} AABB nodes needed");
            }

            return &options->treeNodePool[aabbTreeCount++];
        }

        // State
        float* sortedMins;
        float* sortedMaxs;
        float* sortedCoplanar;
        int aabbTreeCount;
    };
} // namespace

namespace xmodel
{
    std::unique_ptr<CommonCollisionTree> CreateCollisionTree(const IRigidVertListAccessor& vertList)
    {
        auto tree = std::make_unique<CommonCollisionTree>();

        Bounds globalBounds;

        const auto triOffset = vertList.GetTriOffset();
        const auto triCount = vertList.GetTriCount();
        std::vector<Bounds> leafBounds;
        auto lastMergeable = false;

        Bounds prevBounds;
        for (size_t triNumber = 0; triNumber < triCount; triNumber++)
        {
            const auto triIndex = triOffset + triNumber;
            Bounds triBounds;

            triBounds.ExpandFromTri(vertList.GetCoordinatesForTri(triIndex));
            globalBounds.ExpandFromBounds(triBounds);

            auto shouldMerge = false;
            if (lastMergeable)
            {
                const auto prevVolume = prevBounds.GetVolume();
                const auto thisVolume = triBounds.GetVolume();

                prevBounds.ExpandFromBounds(triBounds);
                const auto combinedVolume = prevBounds.GetVolume();
                if (combinedVolume <= prevVolume + thisVolume)
                    shouldMerge = true;
            }

            if (shouldMerge)
            {
                leafBounds.back() = prevBounds;
                assert(!tree->leafs.back().twoTriangles);
                tree->leafs.back().twoTriangles = 1;
                lastMergeable = false;
            }
            else
            {
                tree->leafs.emplace_back(CommonCollisionLeaf{
                    .triangleBeginIndex = static_cast<decltype(CommonCollisionLeaf::triangleBeginIndex)>(triIndex),
                    .twoTriangles = 0,
                });
                leafBounds.emplace_back(triBounds);
                lastMergeable = true;
                prevBounds = triBounds;
            }
        }

        tree->trans = -globalBounds.m_mins;
        const auto globalDelta = globalBounds.GetDelta();
        tree->scale = Eigen::Vector3f(std::numeric_limits<uint16_t>::max(), std::numeric_limits<uint16_t>::max(), std::numeric_limits<uint16_t>::max())
                          .cwiseQuotient(globalDelta);

        AabbTreeBuilder aabbTreeBuilder;
        GenericAabbTreeOptions options{};
        options.maintainValidBounds = 1;
        options.treeNodePool = (GenericAabbTree*)malloc(sizeof(GenericAabbTree) * 0x2000);
        options.treeNodeLimit = 0x2000;
        options.minItemsPerLeaf = 1;
        options.maxItemsPerLeaf = 16;
        options.mins = (float (*)[3])malloc(12 * leafBounds.size());
        options.maxs = (float (*)[3])malloc(12 * leafBounds.size());
        options.items = tree->leafs.data();
        options.itemCount = static_cast<int>(leafBounds.size());
        options.itemSize = 2;

        for (size_t leafIndex = 0; leafIndex < leafBounds.size(); ++leafIndex)
        {
            const auto& bounds = leafBounds[leafIndex];
            options.mins[leafIndex][0] = bounds.m_mins.x();
            options.mins[leafIndex][1] = bounds.m_mins.y();
            options.mins[leafIndex][2] = bounds.m_mins.z();
            options.maxs[leafIndex][0] = bounds.m_maxs.x();
            options.maxs[leafIndex][1] = bounds.m_maxs.y();
            options.maxs[leafIndex][2] = bounds.m_maxs.z();
        }

        const auto nodeCount = options.itemCount > 0 ? aabbTreeBuilder.BuildAabbTree(&options) : 0;

        tree->nodes.resize(nodeCount);
        for (auto nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
        {
            auto& outNode = tree->nodes[nodeIndex];
            const auto& builtNode = options.treeNodePool[nodeIndex];

            const auto leafEnd = builtNode.itemCount + builtNode.firstItem;
            Bounds nodeBounds;

            for (auto leafIndex = builtNode.firstItem; leafIndex < leafEnd; ++leafIndex)
                nodeBounds.ExpandFromBounds(options.mins[leafIndex], options.maxs[leafIndex]);

            outNode.aabb.mins[0] = static_cast<uint16_t>(std::clamp<float>(
                (nodeBounds.m_mins.x() + tree->trans[0]) * tree->scale[0] - 0.5f, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max()));
            outNode.aabb.mins[1] = static_cast<uint16_t>(std::clamp<float>(
                (nodeBounds.m_mins.y() + tree->trans[1]) * tree->scale[1] - 0.5f, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max()));
            outNode.aabb.mins[2] = static_cast<uint16_t>(std::clamp<float>(
                (nodeBounds.m_mins.z() + tree->trans[2]) * tree->scale[2] - 0.5f, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max()));

            outNode.aabb.maxs[0] = static_cast<uint16_t>(std::clamp<float>(
                (nodeBounds.m_maxs.x() + tree->trans[0]) * tree->scale[0] + 0.5f, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max()));
            outNode.aabb.maxs[1] = static_cast<uint16_t>(std::clamp<float>(
                (nodeBounds.m_maxs.y() + tree->trans[1]) * tree->scale[1] + 0.5f, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max()));
            outNode.aabb.maxs[2] = static_cast<uint16_t>(std::clamp<float>(
                (nodeBounds.m_maxs.z() + tree->trans[2]) * tree->scale[2] + 0.5f, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max()));

            if (builtNode.childCount)
            {
                outNode.childBeginIndex = builtNode.firstChild;
                assert(outNode.childBeginIndex == builtNode.firstChild);

                outNode.childCount = builtNode.childCount;
                assert(outNode.childCount == builtNode.childCount);
            }
            else
            {
                assert(builtNode.itemCount);

                outNode.childBeginIndex = builtNode.firstItem;
                assert(outNode.childBeginIndex == builtNode.firstItem);

                outNode.childCount = builtNode.itemCount;
                assert(outNode.childCount == builtNode.itemCount);

                outNode.childrenAreLeafs = true;
            }
        }
        free(options.mins);
        free(options.maxs);
        free(options.treeNodePool);

        return std::move(tree);
    }
} // namespace xmodel
