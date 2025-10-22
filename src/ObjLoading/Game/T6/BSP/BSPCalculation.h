#pragma once

#include "BSP.h"

namespace BSP
{
    enum PlaneAxis
    {
        AXIS_X,
        AXIS_Y,
        AXIS_Z
    };

    enum PlaneSide
    {
        SIDE_FRONT,
        SIDE_BACK,
        SIDE_INTERSECTS
    };

    class BSPObject
    {
    public:
        vec3_t min;
        vec3_t max;
        int partitionIndex; // index of the partition the object is contained in

        BSPObject(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax, int objPartitionIndex);
    };


    class BSPLeaf
    {
    public:
        std::vector<std::shared_ptr<BSPObject>> objectList;

        void addObject(std::shared_ptr<BSPObject> object);
        BSPObject* getObject(int index);
        size_t getObjectCount();
    };

    class BSPTree;

    class BSPNode
    {
    public:
        std::unique_ptr<BSPTree> front;
        std::unique_ptr<BSPTree> back;

        PlaneAxis axis;  // axis that the split plane is on
        float distance; // distance from the origin (0, 0, 0) to the plane

        BSPNode(std::unique_ptr<BSPTree> frontTree, std::unique_ptr<BSPTree> backTree, PlaneAxis nodeAxis, float nodeDistance);
        PlaneSide objectIsInFront(BSPObject* object);
    };

    class BSPTree
    {
    public:
        bool isLeaf;
        std::unique_ptr<BSPLeaf> leaf;
        std::unique_ptr<BSPNode> node;

        int level; // level in the BSP tree
        vec3_t min;
        vec3_t max;

        BSPTree(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax, int treeLevel);
        void splitTree();
        void addObjectToTree(std::shared_ptr<BSPObject> object);
    };
}