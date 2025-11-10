#pragma once

#include "BSP.h"

#include <cstdint>

namespace BSP
{
    enum class PlaneAxis : std::uint8_t
    {
        AXIS_X,
        AXIS_Y,
        AXIS_Z
    };

    enum class PlaneSide : std::uint8_t
    {
        SIDE_FRONT,
        SIDE_BACK,
        SIDE_INTERSECTS
    };

    class BSPObject
    {
    public:
        T6::vec3_t min;
        T6::vec3_t max;
        int partitionIndex; // index of the partition the object is contained in

        BSPObject(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax, int objPartitionIndex);
    };

    class BSPLeaf
    {
    public:
        std::vector<std::shared_ptr<BSPObject>> objectList;

        void addObject(std::shared_ptr<BSPObject> object);
        [[nodiscard]] BSPObject* getObject(size_t index) const;
        [[nodiscard]] size_t getObjectCount() const;
    };

    class BSPTree;

    class BSPNode
    {
    public:
        std::unique_ptr<BSPTree> front;
        std::unique_ptr<BSPTree> back;

        PlaneAxis axis; // axis that the split plane is on
        float distance; // distance from the origin (0, 0, 0) to the plane

        BSPNode(std::unique_ptr<BSPTree> frontTree, std::unique_ptr<BSPTree> backTree, PlaneAxis nodeAxis, float nodeDistance);
        [[nodiscard]] PlaneSide objectIsInFront(const BSPObject& object) const;
    };

    class BSPTree
    {
    public:
        bool isLeaf;
        std::unique_ptr<BSPLeaf> leaf;
        std::unique_ptr<BSPNode> node;

        int level; // level in the BSP tree
        T6::vec3_t min;
        T6::vec3_t max;

        BSPTree(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax, int treeLevel);
        void splitTree();
        void addObjectToTree(std::shared_ptr<BSPObject> object) const;
    };
} // namespace BSP
