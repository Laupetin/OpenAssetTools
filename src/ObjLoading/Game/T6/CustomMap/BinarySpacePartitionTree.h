#pragma once

#include <memory>
#include <vector>

#include "Game/T6/T6.h"
using namespace T6;

constexpr int MAX_NODE_SIZE = 512; // maximum size a BSP node can be before it becomes a leaf

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

class Object
{
public:
    vec3_t min;
    vec3_t max;
    int partitionIndex; // index of the partition the object is contained in

    Object(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax, int objPartitionIndex);
};


class BSPLeaf
{
public:
    std::vector<std::shared_ptr<Object>> objectList;

    void addObject(std::shared_ptr<Object> object);
    Object* getObject(int index);
    int getObjectCount();
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
    PlaneSide objectIsInfront(Object* object);
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
    void addObjectToTree(std::shared_ptr<Object> object);
};
