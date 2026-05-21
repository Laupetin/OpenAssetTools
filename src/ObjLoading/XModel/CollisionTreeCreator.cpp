#include "CollisionTreeCreator.h"

#include <cassert>

namespace
{
    class Bounds
    {
    public:
        Bounds()
            : m_mins(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
              m_maxs(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min())
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
            m_maxs[0] = std::min(m_maxs[0], otherBounds.m_maxs[0]);
            m_maxs[1] = std::min(m_maxs[1], otherBounds.m_maxs[1]);
            m_maxs[2] = std::min(m_maxs[2], otherBounds.m_maxs[2]);
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

        // TODO: Calculate nodes

        return std::move(tree);
    }
} // namespace xmodel
