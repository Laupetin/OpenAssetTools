#include "CollisionTreeCreator.h"

#include "Utils/Logging/Log.h"

#include <algorithm>
#include <cassert>

namespace
{
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

        [[nodiscard]] Eigen::Vector3f GetDelta() const
        {
            return m_maxs - m_mins;
        }

        [[nodiscard]] float GetVolume() const
        {
            return GetDelta().prod();
        }

        [[nodiscard]] float AddedVolume(const Bounds& addedBounds) const
        {
            Bounds expandedBounds(*this);
            expandedBounds.ExpandFromBounds(addedBounds);

            return expandedBounds.GetVolume() - GetVolume();
        }

        Eigen::Vector3f m_mins;
        Eigen::Vector3f m_maxs;
    };

    constexpr auto MIN_ITEMS_PER_LEAF = 1u;
    constexpr auto MAX_ITEMS_PER_LEAF = 16u;

    struct GenericAabbTree
    {
        size_t firstItem;
        size_t itemCount;
        size_t firstChild;
        size_t childCount;
    };

    class AabbTreeBuilder
    {
    public:
        AabbTreeBuilder(std::vector<GenericAabbTree>& nodes, std::vector<xmodel::CommonCollisionLeaf>& items, std::vector<Bounds>& itemBounds)
            : m_nodes(nodes),
              m_items(items),
              m_item_bounds(itemBounds)
        {
        }

        size_t BuildAabbTree()
        {
            if (m_items.empty())
                return 0;

            const auto itemCount = m_items.size();

            std::vector<size_t> remap(m_items.size());
            m_sorted_mins = std::vector<float>(m_items.size());
            m_sorted_maxs = std::vector<float>(m_items.size());
            m_sorted_coplanar = std::vector<float>(m_items.size());

            std::ranges::iota(remap, 0);

            // Insert root node
            m_nodes.emplace_back(GenericAabbTree{
                .firstItem = 0,
                .itemCount = itemCount,
                .firstChild = 0,
                .childCount = 0,
            });

            BuildAabbTree_r(0, remap.data());

            // Reorder items
            std::vector<xmodel::CommonCollisionLeaf> itemCopies(itemCount);
            memcpy(itemCopies.data(), m_items.data(), sizeof(xmodel::CommonCollisionLeaf) * itemCount);
            for (size_t itemIndex = 0; itemIndex < itemCount; ++itemIndex)
                m_items[itemIndex] = itemCopies[remap[itemIndex]];

            // Always trying to maintain valid bounds
            std::vector<Bounds> boundCopies(itemCount);
            std::ranges::copy(m_item_bounds, boundCopies.begin());
            for (size_t itemIndex = 0; itemIndex < itemCount; ++itemIndex)
                m_item_bounds[itemIndex] = boundCopies[remap[itemIndex]];

            return m_nodes.size();
        }

    private:
        void BuildAabbTree_r(size_t treeIndex, size_t* remap)
        {
            assert(m_nodes[treeIndex].itemCount);

            m_nodes[treeIndex].firstChild = m_nodes.size();
            m_nodes[treeIndex].childCount = 0;

            size_t midStart, lastStart;
            if (m_nodes[treeIndex].itemCount > MAX_ITEMS_PER_LEAF && SplitAabbTree(m_nodes[treeIndex].itemCount, remap, &midStart, &lastStart))
            {
                const auto subTreeStartIndex = m_nodes.size();
                assert(m_nodes[treeIndex].firstChild == subTreeStartIndex);

                CreateAabbSubTrees(treeIndex, remap, 0, midStart);

                if (midStart < lastStart)
                    CreateAabbSubTrees(treeIndex, remap, midStart, lastStart - midStart);

                CreateAabbSubTrees(treeIndex, remap, lastStart, m_nodes[treeIndex].itemCount - lastStart);

                m_nodes[treeIndex].childCount = m_nodes.size() - m_nodes[treeIndex].firstChild;
                for (size_t childIndex = 0; childIndex < m_nodes[treeIndex].childCount; ++childIndex)
                {
                    const auto subTreeIndex = subTreeStartIndex + childIndex;
                    BuildAabbTree_r(subTreeIndex, &remap[m_nodes[subTreeIndex].firstItem - m_nodes[treeIndex].firstItem]);
                }
            }
        }

        int SplitAabbTree(const size_t count, size_t* remap, size_t* midStart, size_t* lastStart)
        {
            size_t swapCache;

            int splitAxis;
            float splitDist;
            if (!PickAabbSplitPlane(m_item_bounds.data(), remap, count, splitAxis, splitDist))
                return 0;

            int bot = 0;
            int top = static_cast<int>(count - 1);
            Bounds bounds[2];
            while (bot <= top)
            {
                while (bot <= top && splitDist >= m_item_bounds[remap[bot]].m_maxs[splitAxis] && splitDist > m_item_bounds[remap[bot]].m_mins[splitAxis])
                {
                    bounds[0].ExpandFromBounds(m_item_bounds[remap[bot]]);
                    ++bot;
                }

                while (bot <= top && m_item_bounds[remap[top]].m_mins[splitAxis] >= splitDist && m_item_bounds[remap[top]].m_maxs[splitAxis] > splitDist)
                {
                    bounds[1].ExpandFromBounds(m_item_bounds[remap[top]]);
                    --top;
                }

                if (bot > top)
                    break;

                if ((m_item_bounds[remap[bot]].m_mins[splitAxis] < splitDist || m_item_bounds[remap[bot]].m_maxs[splitAxis] <= splitDist)
                    && (splitDist < m_item_bounds[remap[top]].m_maxs[splitAxis] || splitDist <= m_item_bounds[remap[top]].m_mins[splitAxis]))
                {
                    int mid;
                    for (mid = bot; mid < top; ++mid)
                    {
                        if (m_item_bounds[remap[mid]].m_mins[splitAxis] >= splitDist && m_item_bounds[remap[mid]].m_maxs[splitAxis] > splitDist)
                        {
                            swapCache = remap[mid];
                            remap[mid] = remap[top];
                            remap[top] = swapCache;
                            break;
                        }

                        if (splitDist >= m_item_bounds[remap[mid]].m_maxs[splitAxis] && splitDist > m_item_bounds[remap[mid]].m_mins[splitAxis])
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

            if (bot <= top && (bot < MIN_ITEMS_PER_LEAF || top - bot + 1 < MIN_ITEMS_PER_LEAF || count - top - 1 < MIN_ITEMS_PER_LEAF))
            {
                while (bot <= top)
                {
                    while (bot <= top)
                    {
                        if (bounds[1].AddedVolume(m_item_bounds[remap[bot]]) < bounds[0].AddedVolume(m_item_bounds[remap[bot]]))
                            break;

                        bounds[0].ExpandFromBounds(m_item_bounds[remap[bot]]);
                        ++bot;
                    }

                    while (bot <= top)
                    {
                        if (bounds[0].AddedVolume(m_item_bounds[remap[top]]) < bounds[1].AddedVolume(m_item_bounds[remap[top]]))
                            break;

                        bounds[1].ExpandFromBounds(m_item_bounds[remap[top]]);
                        --top;
                    }

                    if (bot >= top)
                    {
                        if (bot == top)
                        {
                            if (static_cast<size_t>(2 * bot) >= count)
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

        bool PickAabbSplitPlane(const Bounds* bounds, const size_t* remap, const size_t count, int& chosenAxis, float& chosenDist)
        {
            Bounds globalBounds;

            for (size_t i = 0; i < count; ++i)
                globalBounds.ExpandFromBounds(bounds[remap[i]]);

            size_t smallestAxis = globalBounds.m_maxs[0] - globalBounds.m_mins[0] > globalBounds.m_maxs[1] - globalBounds.m_mins[1];
            if (globalBounds.m_maxs[smallestAxis] - globalBounds.m_mins[smallestAxis] > globalBounds.m_maxs[2] - globalBounds.m_mins[2])
                smallestAxis = 2;

            int axisBias[3];
            for (size_t i = 0; i < 3u; ++i)
            {
                axisBias[i] = static_cast<int>((globalBounds.m_maxs[i] - globalBounds.m_mins[i] + 1.0f) * 10.0f
                                                   / (globalBounds.m_maxs[smallestAxis] - globalBounds.m_mins[smallestAxis] + 1.0f)
                                               + 0.4999999990686774);
            }

            auto bestHeuristic = -1;
            for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
            {
                size_t minMaxCount = 0;
                size_t coplanarCount = 0;

                for (size_t i = 0; i < count; ++i)
                {
                    if (bounds[remap[i]].m_mins[axisIndex] == bounds[remap[i]].m_maxs[axisIndex])
                    {
                        m_sorted_coplanar[coplanarCount++] = bounds[remap[i]].m_mins[axisIndex];
                    }
                    else
                    {
                        m_sorted_mins[minMaxCount] = bounds[remap[i]].m_mins[axisIndex];
                        m_sorted_maxs[minMaxCount++] = bounds[remap[i]].m_maxs[axisIndex];
                    }
                }

                std::sort(&m_sorted_mins[0], &m_sorted_mins[minMaxCount], std::greater());
                std::sort(&m_sorted_maxs[0], &m_sorted_maxs[minMaxCount], std::greater());
                std::sort(&m_sorted_coplanar[0], &m_sorted_coplanar[coplanarCount], std::greater());

                int sideFrontCount = 0;
                int sideBackCount = static_cast<int>(count);
                int sideSplitCount = 0;
                int sideOnCount = 0;
                int prevMinCount = 0;
                int prevOnCount = 0;

                float nextDist;
                if (coplanarCount && m_sorted_coplanar[0] - m_sorted_mins[0] < 0.0f)
                    nextDist = m_sorted_coplanar[0];
                else
                    nextDist = m_sorted_mins[0];

                size_t minIndex = 0;
                size_t maxIndex = 0;
                size_t onIndex = 0;
                while (nextDist < std::numeric_limits<float>::max())
                {
                    const auto dist = nextDist;
                    nextDist = std::numeric_limits<float>::max();

                    sideSplitCount += prevMinCount;
                    sideBackCount -= prevMinCount;
                    prevMinCount = 0;

                    while (minIndex < minMaxCount && m_sorted_mins[minIndex] == dist)
                    {
                        ++prevMinCount;
                        ++minIndex;
                    }

                    if (minIndex < minMaxCount && m_sorted_mins[minIndex] < nextDist)
                        nextDist = m_sorted_mins[minIndex];

                    while (maxIndex < minMaxCount && m_sorted_maxs[maxIndex] == dist)
                    {
                        ++sideFrontCount;
                        --sideSplitCount;
                        ++maxIndex;
                    }

                    if (maxIndex < minMaxCount && nextDist > m_sorted_maxs[maxIndex])
                        nextDist = m_sorted_maxs[maxIndex];

                    sideFrontCount += prevOnCount;
                    sideOnCount -= prevOnCount;
                    prevOnCount = 0;

                    while (onIndex < coplanarCount && m_sorted_coplanar[onIndex] == dist)
                    {
                        ++prevOnCount;
                        ++onIndex;
                    }

                    sideOnCount += prevOnCount;
                    sideBackCount -= prevOnCount;

                    if (onIndex < coplanarCount && nextDist > m_sorted_coplanar[onIndex])
                        nextDist = m_sorted_coplanar[onIndex];

                    assert(sideFrontCount + sideBackCount + sideSplitCount + sideOnCount == count);
                    assert(sideFrontCount >= 0);
                    assert(sideBackCount >= 0);
                    assert(sideSplitCount >= 0);
                    assert(sideOnCount >= 0);

                    if (sideFrontCount > 1 && sideBackCount > 1)
                    {
                        int heuristic =
                            axisBias[axisIndex] + static_cast<int>(count) - std::abs(sideFrontCount - sideBackCount) - sideOnCount - 4 * sideSplitCount;

                        if (!sideOnCount && !sideSplitCount && !prevMinCount)
                            heuristic += static_cast<int>(nextDist - dist);

                        if (heuristic > bestHeuristic)
                        {
                            bestHeuristic = heuristic;
                            chosenAxis = axisIndex;
                            if (sideOnCount || sideSplitCount || prevMinCount)
                                chosenDist = dist;
                            else
                                chosenDist = (dist + nextDist) * 0.5f;
                        }
                    }
                }
            }

            return bestHeuristic != -1;
        }

        void CreateAabbSubTrees(const size_t treeIndex, size_t* remap, const size_t firstIndex, const size_t count)
        {
            size_t midStart, lastStart;
            if (count > MAX_ITEMS_PER_LEAF && SplitAabbTree(count, &remap[firstIndex], &midStart, &lastStart))
            {
                m_nodes.emplace_back(GenericAabbTree{
                    .firstItem = firstIndex + m_nodes[treeIndex].firstItem,
                    .itemCount = midStart,
                    .firstChild = 0,
                    .childCount = 0,
                });

                if (midStart < lastStart)
                {
                    m_nodes.emplace_back(GenericAabbTree{
                        .firstItem = midStart + firstIndex + m_nodes[treeIndex].firstItem,
                        .itemCount = lastStart - midStart,
                        .firstChild = 0,
                        .childCount = 0,
                    });
                }

                m_nodes.emplace_back(GenericAabbTree{
                    .firstItem = lastStart + firstIndex + m_nodes[treeIndex].firstItem,
                    .itemCount = count - lastStart,
                    .firstChild = 0,
                    .childCount = 0,
                });
            }
            else
            {
                m_nodes.emplace_back(GenericAabbTree{
                    .firstItem = firstIndex + m_nodes[treeIndex].firstItem,
                    .itemCount = count,
                    .firstChild = 0,
                    .childCount = 0,
                });
            }
        }

        // Options
        std::vector<GenericAabbTree>& m_nodes;
        std::vector<xmodel::CommonCollisionLeaf>& m_items;
        std::vector<Bounds>& m_item_bounds;

        // State
        std::vector<float> m_sorted_mins;
        std::vector<float> m_sorted_maxs;
        std::vector<float> m_sorted_coplanar;
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

        std::vector<GenericAabbTree> nodes;
        AabbTreeBuilder aabbTreeBuilder(nodes, tree->leafs, leafBounds);
        aabbTreeBuilder.BuildAabbTree();

        const auto nodeCount = nodes.size();
        tree->nodes.resize(nodeCount);
        for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
        {
            auto& outNode = tree->nodes[nodeIndex];
            const auto& builtNode = nodes[nodeIndex];

            const auto leafEnd = builtNode.itemCount + builtNode.firstItem;
            Bounds nodeBounds;

            for (auto leafIndex = builtNode.firstItem; leafIndex < leafEnd; ++leafIndex)
                nodeBounds.ExpandFromBounds(leafBounds[leafIndex]);

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
                assert(builtNode.firstChild <= std::numeric_limits<decltype(outNode.childBeginIndex)>::max());
                outNode.childBeginIndex = static_cast<decltype(outNode.childBeginIndex)>(builtNode.firstChild);

                assert(builtNode.childCount <= std::numeric_limits<decltype(outNode.childCount)>::max());
                outNode.childCount = static_cast<decltype(outNode.childCount)>(builtNode.childCount);
            }
            else
            {
                assert(builtNode.itemCount);

                assert(builtNode.firstItem <= std::numeric_limits<decltype(outNode.childBeginIndex)>::max());
                outNode.childBeginIndex = static_cast<decltype(outNode.childBeginIndex)>(builtNode.firstItem);

                assert(builtNode.itemCount <= std::numeric_limits<decltype(outNode.childCount)>::max());
                outNode.childCount = static_cast<decltype(outNode.childCount)>(builtNode.itemCount);

                outNode.childrenAreLeafs = true;
            }
        }

        return std::move(tree);
    }
} // namespace xmodel
