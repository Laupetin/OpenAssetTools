#pragma once

#include <Eigen>
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace xmodel
{
    struct CommonCollisionAabb
    {
        std::array<uint16_t, 3> mins;
        std::array<uint16_t, 3> maxs;
    };

    struct CommonCollisionNode
    {
        CommonCollisionAabb aabb;
        uint16_t childBeginIndex;
        uint16_t childCount : 15;
        uint16_t childrenAreLeafs : 1;
    };

    struct CommonCollisionLeaf
    {
        uint16_t triangleBeginIndex : 15;
        uint16_t twoTriangles : 1;
    };

    struct CommonCollisionTree
    {
        Eigen::Vector3f trans;
        Eigen::Vector3f scale;
        std::vector<CommonCollisionNode> nodes;
        std::vector<CommonCollisionLeaf> leafs;
    };

    class IRigidVertListAccessor
    {
    public:
        IRigidVertListAccessor() = default;
        virtual ~IRigidVertListAccessor() = default;
        IRigidVertListAccessor(const IRigidVertListAccessor& other) = default;
        IRigidVertListAccessor(IRigidVertListAccessor&& other) noexcept = default;
        IRigidVertListAccessor& operator=(const IRigidVertListAccessor& other) = default;
        IRigidVertListAccessor& operator=(IRigidVertListAccessor&& other) noexcept = default;

        [[nodiscard]] virtual size_t GetTriOffset() const = 0;
        [[nodiscard]] virtual size_t GetTriCount() const = 0;
        [[nodiscard]] virtual std::array<Eigen::Vector3f, 3> GetCoordinatesForTri(size_t triIndex) const = 0;
    };

    std::unique_ptr<CommonCollisionTree> CreateCollisionTree(const IRigidVertListAccessor& vertList);
} // namespace xmodel
