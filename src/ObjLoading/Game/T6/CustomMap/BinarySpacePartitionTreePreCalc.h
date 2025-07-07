#pragma once

/*
Heavily modified version of https://github.com/sudeshnapal12/Space-Partitioning-Algorithms BSP implementation
Credit to sudeshnapal12


Precalculated, evenly sized BSPs are much more efficient and smaller compared to dynamically creating them
*/

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#define MAX_AABB_SIZE 512 // maximum size an AABB tree can be before it becomes a node

enum PlaneAxis
{
    AXIS_X,
    AXIS_Y,
    AXIS_Z
};

class Object
{
public:
    double low[3];
    double high[3];

    // custom data
    int partitionIndex; // index of the partition the object is based on (custom)

    Object(double min_x, double min_y, double min_z, double max_x, double max_y, double max_z, int partition_Index)
    {
        low[0] = min_x;
        low[1] = min_y, low[2] = min_z;
        high[0] = max_x;
        high[1] = max_y;
        high[2] = max_z;
        partitionIndex = partition_Index;
    }
};

union u_BSPNode;
class BSPTree;

class BSPLeaf
{
private:
    std::vector<Object*> objectList;

public:
    BSPLeaf()
    {
        objectList = std::vector<Object*>();
    }

    ~BSPLeaf()
    {
        objectList.clear();
    }

    void addToList(Object* object)
    {
        objectList.push_back(object);
    }

    int getObjectCount()
    {
        return objectList.size();
    }

    Object* getObject(int index)
    {
        return objectList.at(index);
    }
};

enum objectPlaneSide
{
    SIDE_FRONT,
    SIDE_BACK,
    SIDE_INTERSECTS
};

class BSPNode
{
public:
    BSPTree* front;
    BSPTree* back;

    PlaneAxis axis;  // axis that the split plane is on
    double distance; // distance from the origin (0, 0, 0) to the plane

    BSPNode(BSPTree* _front, BSPTree* _back, PlaneAxis _axis, double _distance)
    {
        front = _front;
        back = _back;
        axis = _axis;
        distance = _distance;
    }

    objectPlaneSide objectIsInfront(Object* object)
    {
        double minCoord, maxCoord;

        // Select the relevant coordinate based on the plane's axis
        switch (axis)
        {
        case AXIS_X:
            minCoord = object->low[0];
            maxCoord = object->high[0];
            break;
        case AXIS_Y:
            minCoord = object->low[1];
            maxCoord = object->high[1];
            break;
        case AXIS_Z:
            minCoord = object->low[2];
            maxCoord = object->high[2];
            break;
        default:
            _ASSERT(false); // this should never be executed
        }

        // Compare with the plane's distance
        if (maxCoord < distance)
        {
            return SIDE_BACK; // Object is entirely on the negative side
        }
        else if (minCoord > distance)
        {
            return SIDE_FRONT; // Object is entirely on the positive side
        }
        else
        {
            return SIDE_INTERSECTS;
        }
    }
};

union u_BSPNode
{
    BSPLeaf* leaf;
    BSPNode* node;
};

class BSPTree
{
public:
    bool isLeaf;
    u_BSPNode u;

    int level;      // level in the BSP tree
    double low[3];  // mins
    double high[3]; // maxs

    BSPTree(double min_x, double min_y, double min_z, double max_x, double max_y, double max_z, int _level)
    {
        low[0] = min_x;
        low[1] = min_y, low[2] = min_z;
        high[0] = max_x;
        high[1] = max_y;
        high[2] = max_z;
        level = _level;
        splitTree();
    }

    // For simplicity, only split across the X and Z axis.
    // It is unlikely that there are many layers to a map, and is instead mostly flat
    void splitTree()
    {
        BSPTree* front;
        BSPTree* back;
        double halfLength;

        if (high[0] - low[0] > MAX_AABB_SIZE)
        {
            // split along the x axis
            halfLength = (low[0] + high[0]) * 0.5f;
            front = new BSPTree(halfLength, low[1], low[2], high[0], high[1], high[2], level + 1);
            back = new BSPTree(low[0], low[1], low[2], halfLength, high[1], high[2], level + 1);

            isLeaf = false;
            u.node = new BSPNode(front, back, AXIS_X, halfLength);
        }
        else if (high[2] - low[2] > MAX_AABB_SIZE)
        {
            // split along the z axis
            halfLength = (low[2] + high[2]) * 0.5f;
            front = new BSPTree(low[0], low[1], halfLength, high[0], high[1], high[2], level + 1);
            back = new BSPTree(low[0], low[1], low[2], high[0], high[1], halfLength, level + 1);

            isLeaf = false;
            u.node = new BSPNode(front, back, AXIS_Z, halfLength);
        }
        else
        {
            isLeaf = true;
            u.leaf = new BSPLeaf();
        }
    }

    void addObject(Object* object)
    {
        if (isLeaf)
        {
            u.leaf->addToList(object);
        }
        else
        {
            objectPlaneSide side = u.node->objectIsInfront(object);
            if (side == SIDE_FRONT)
            {
                u.node->front->addObject(object);
            }
            else if (side == SIDE_BACK)
            {
                u.node->back->addObject(object);
            }
            else // intersects
            {
                u.node->front->addObject(object);
                u.node->back->addObject(object);
            }
        }
    }
};
