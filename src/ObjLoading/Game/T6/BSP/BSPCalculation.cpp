#include "BSPCalculation.h"

namespace BSP
{
    constexpr int MAX_NODE_SIZE = 512; // maximum size a BSP node can be before it becomes a leaf

    BSPObject::BSPObject(
        const float xMin, const float yMin, const float zMin, const float xMax, const float yMax, const float zMax, const int objPartitionIndex)
    {
        min.x = xMin;
        min.y = yMin;
        min.z = zMin;
        max.x = xMax;
        max.y = yMax;
        max.z = zMax;
        partitionIndex = objPartitionIndex;
    }

    void BSPLeaf::addObject(std::shared_ptr<BSPObject> object)
    {
        objectList.emplace_back(std::move(object));
    }

    BSPObject* BSPLeaf::getObject(const size_t index) const
    {
        return objectList.at(index).get();
    }

    size_t BSPLeaf::getObjectCount() const
    {
        return objectList.size();
    }

    BSPNode::BSPNode(std::unique_ptr<BSPTree> frontTree, std::unique_ptr<BSPTree> backTree, const PlaneAxis nodeAxis, const float nodeDistance)
    {
        front = std::move(frontTree);
        back = std::move(backTree);
        axis = nodeAxis;
        distance = nodeDistance;
    }

    PlaneSide BSPNode::objectIsInFront(const BSPObject& object) const
    {
        float minCoord, maxCoord;

        // Select the relevant coordinate based on the plane's axis
        if (axis == AXIS_X)
        {
            minCoord = object.min.x;
            maxCoord = object.max.x;
        }
        else if (axis == AXIS_Y)
        {
            minCoord = object.min.y;
            maxCoord = object.max.y;
        }
        else // axis == AXIS_Z
        {
            minCoord = object.min.z;
            maxCoord = object.max.z;
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

    BSPTree::BSPTree(const float xMin, const float yMin, const float zMin, const float xMax, const float yMax, const float zMax, const int treeLevel)
    {
        min.x = xMin;
        min.y = yMin;
        min.z = zMin;
        max.x = xMax;
        max.y = yMax;
        max.z = zMax;
        level = treeLevel;
        splitTree();
    }

    void BSPTree::splitTree()
    {
        std::unique_ptr<BSPTree> front;
        std::unique_ptr<BSPTree> back;
        float halfLength;

        if (max.x - min.x > MAX_NODE_SIZE)
        {
            // split along the x axis
            halfLength = (min.x + max.x) * 0.5f;
            front = std::make_unique<BSPTree>(halfLength, min.y, min.z, max.x, max.y, max.z, level + 1);
            back = std::make_unique<BSPTree>(min.x, min.y, min.z, halfLength, max.y, max.z, level + 1);

            isLeaf = false;
            node = std::make_unique<BSPNode>(std::move(front), std::move(back), AXIS_X, halfLength);
            leaf = nullptr;
        }
        else if (max.y - min.y > MAX_NODE_SIZE)
        {
            // split along the x axis
            halfLength = (min.y + max.y) * 0.5f;
            front = std::make_unique<BSPTree>(min.x, halfLength, min.z, max.x, max.y, max.z, level + 1);
            back = std::make_unique<BSPTree>(min.x, min.y, min.z, max.x, halfLength, max.z, level + 1);

            isLeaf = false;
            node = std::make_unique<BSPNode>(std::move(front), std::move(back), AXIS_Y, halfLength);
            leaf = nullptr;
        }
        else if (max.z - min.z > MAX_NODE_SIZE)
        {
            // split along the z axis
            halfLength = (min.z + max.z) * 0.5f;
            front = std::make_unique<BSPTree>(min.x, min.y, halfLength, max.x, max.y, max.z, level + 1);
            back = std::make_unique<BSPTree>(min.x, min.y, min.z, max.x, max.y, halfLength, level + 1);

            isLeaf = false;
            node = std::make_unique<BSPNode>(std::move(front), std::move(back), AXIS_Z, halfLength);
            leaf = nullptr;
        }
        else
        {
            isLeaf = true;
            node = nullptr;
            leaf = std::make_unique<BSPLeaf>();
        }
    }

    void BSPTree::addObjectToTree(std::shared_ptr<BSPObject> object) const
    {
        if (isLeaf)
        {
            leaf->addObject(std::move(object));
        }
        else
        {
            const PlaneSide side = node->objectIsInFront(*object);
            if (side == SIDE_FRONT)
            {
                node->front->addObjectToTree(std::move(object));
            }
            else if (side == SIDE_BACK)
            {
                node->back->addObjectToTree(std::move(object));
            }
            else // intersects
            {
                node->front->addObjectToTree(object);
                node->back->addObjectToTree(object);
            }
        }
    }
} // namespace BSP
