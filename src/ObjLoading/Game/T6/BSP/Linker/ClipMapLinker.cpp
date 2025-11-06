#include "ClipMapLinker.h"

#include "Game/T6/BSP/BSPUtil.h"

using namespace T6;

namespace BSP
{
    ClipMapLinker::ClipMapLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
          m_search_path(searchPath),
          m_context(context)
    {
    }

    void ClipMapLinker::loadDynEnts(clipMap_t* clipMap)
    {
        int dynEntCount = 0;
        clipMap->originalDynEntCount = dynEntCount;
        clipMap->dynEntCount[0] = clipMap->originalDynEntCount + 256; // the game allocs 256 empty dynents, as they may be used ingame
        clipMap->dynEntCount[1] = 0;
        clipMap->dynEntCount[2] = 0;
        clipMap->dynEntCount[3] = 0;

        clipMap->dynEntClientList[0] = m_memory.Alloc<DynEntityClient>(clipMap->dynEntCount[0]);
        clipMap->dynEntClientList[1] = nullptr;

        clipMap->dynEntServerList[0] = nullptr;
        clipMap->dynEntServerList[1] = nullptr;

        clipMap->dynEntCollList[0] = m_memory.Alloc<DynEntityColl>(clipMap->dynEntCount[0]);
        clipMap->dynEntCollList[1] = nullptr;
        clipMap->dynEntCollList[2] = nullptr;
        clipMap->dynEntCollList[3] = nullptr;

        clipMap->dynEntPoseList[0] = m_memory.Alloc<DynEntityPose>(clipMap->dynEntCount[0]);
        clipMap->dynEntPoseList[1] = nullptr;

        clipMap->dynEntDefList[0] = m_memory.Alloc<DynEntityDef>(clipMap->dynEntCount[0]);
        clipMap->dynEntDefList[1] = nullptr;
    }

    void ClipMapLinker::loadVisibility(clipMap_t* clipMap)
    {
        // Only use one visbility cluster for the entire map
        clipMap->numClusters = 1;
        clipMap->vised = 0;
        clipMap->clusterBytes = ((clipMap->numClusters + 63) >> 3) & 0xFFFFFFF8;
        clipMap->visibility = m_memory.Alloc<char>(clipMap->clusterBytes);
        // Official maps set visibility to all 0xFF
        memset(clipMap->visibility, 0xFF, clipMap->clusterBytes);
    }

    void ClipMapLinker::loadBoxData(clipMap_t* clipMap)
    {
        // box_model and box_brush are what are used by game traces as "temporary" collision when
        //  no brush or model is specified to do the trace with.
        // All values in this function are taken from official map BSPs

        // for some reason the maxs are negative, and mins are positive
        // float box_mins = 3.4028235e38;
        // float box_maxs = -3.4028235e38;
        // hack: the floats above can't be safely converted to 32 bit floats, and the game requires them to be exact
        //  so we use the hex representation and set it using int pointers.
        unsigned int box_mins = 0x7F7FFFFF;
        unsigned int box_maxs = 0xFF7FFFFF;
        *(reinterpret_cast<unsigned int*>(&clipMap->box_model.leaf.mins.x)) = box_mins;
        *(reinterpret_cast<unsigned int*>(&clipMap->box_model.leaf.mins.y)) = box_mins;
        *(reinterpret_cast<unsigned int*>(&clipMap->box_model.leaf.mins.z)) = box_mins;
        *(reinterpret_cast<unsigned int*>(&clipMap->box_model.leaf.maxs.x)) = box_maxs;
        *(reinterpret_cast<unsigned int*>(&clipMap->box_model.leaf.maxs.y)) = box_maxs;
        *(reinterpret_cast<unsigned int*>(&clipMap->box_model.leaf.maxs.z)) = box_maxs;

        clipMap->box_model.leaf.brushContents = -1;
        clipMap->box_model.leaf.terrainContents = 0;
        clipMap->box_model.leaf.cluster = 0;
        clipMap->box_model.leaf.collAabbCount = 0;
        clipMap->box_model.leaf.firstCollAabbIndex = 0;
        clipMap->box_model.leaf.leafBrushNode = 0;
        clipMap->box_model.mins.x = 0.0f;
        clipMap->box_model.mins.y = 0.0f;
        clipMap->box_model.mins.z = 0.0f;
        clipMap->box_model.maxs.x = 0.0f;
        clipMap->box_model.maxs.y = 0.0f;
        clipMap->box_model.maxs.z = 0.0f;
        clipMap->box_model.radius = 0.0f;
        clipMap->box_model.info = nullptr;

        clipMap->box_brush = m_memory.Alloc<cbrush_t>();
        clipMap->box_brush->axial_sflags[0][0] = -1;
        clipMap->box_brush->axial_sflags[0][1] = -1;
        clipMap->box_brush->axial_sflags[0][2] = -1;
        clipMap->box_brush->axial_sflags[1][0] = -1;
        clipMap->box_brush->axial_sflags[1][1] = -1;
        clipMap->box_brush->axial_sflags[1][2] = -1;
        clipMap->box_brush->axial_cflags[0][0] = -1;
        clipMap->box_brush->axial_cflags[0][1] = -1;
        clipMap->box_brush->axial_cflags[0][2] = -1;
        clipMap->box_brush->axial_cflags[1][0] = -1;
        clipMap->box_brush->axial_cflags[1][1] = -1;
        clipMap->box_brush->axial_cflags[1][2] = -1;
        clipMap->box_brush->contents = -1;
        clipMap->box_brush->mins.x = 0.0f;
        clipMap->box_brush->mins.y = 0.0f;
        clipMap->box_brush->mins.z = 0.0f;
        clipMap->box_brush->maxs.x = 0.0f;
        clipMap->box_brush->maxs.y = 0.0f;
        clipMap->box_brush->maxs.z = 0.0f;
        clipMap->box_brush->numsides = 0;
        clipMap->box_brush->numverts = 0;
        clipMap->box_brush->sides = nullptr;
        clipMap->box_brush->verts = nullptr;
    }

    void ClipMapLinker::loadRopesAndConstraints(clipMap_t* clipMap)
    {
        clipMap->num_constraints = 0; // max 511
        clipMap->constraints = nullptr;

        // The game allocates 32 empty ropes
        clipMap->max_ropes = 32; // max 300
        clipMap->ropes = m_memory.Alloc<rope_t>(clipMap->max_ropes);
    }

    void ClipMapLinker::loadSubModelCollision(clipMap_t* clipMap, BSPData* bsp)
    {
        // Submodels are used for the world and map ent collision (triggers, bomb zones, etc)
        auto gfxWorldAsset = m_context.LoadDependency<AssetGfxWorld>(bsp->bspName);
        assert(gfxWorldAsset != nullptr);
        GfxWorld* gfxWorld = gfxWorldAsset->Asset();

        // Right now there is only one submodel, the world sub model
        assert(gfxWorld->modelCount == 1);

        clipMap->numSubModels = 1;
        clipMap->cmodels = m_memory.Alloc<cmodel_t>(clipMap->numSubModels);

        GfxBrushModel* gfxModel = &gfxWorld->models[0];
        clipMap->cmodels[0].mins.x = gfxModel->bounds[0].x;
        clipMap->cmodels[0].mins.y = gfxModel->bounds[0].y;
        clipMap->cmodels[0].mins.z = gfxModel->bounds[0].z;
        clipMap->cmodels[0].maxs.x = gfxModel->bounds[1].x;
        clipMap->cmodels[0].maxs.y = gfxModel->bounds[1].y;
        clipMap->cmodels[0].maxs.z = gfxModel->bounds[1].z;
        clipMap->cmodels[0].radius = BSPUtil::distBetweenPoints(clipMap->cmodels[0].mins, clipMap->cmodels[0].maxs) / 2;

        // The world sub model has no leafs associated with it
        clipMap->cmodels[0].leaf.firstCollAabbIndex = 0;
        clipMap->cmodels[0].leaf.collAabbCount = 0;
        clipMap->cmodels[0].leaf.brushContents = 0;
        clipMap->cmodels[0].leaf.terrainContents = 0;
        clipMap->cmodels[0].leaf.mins.x = 0.0f;
        clipMap->cmodels[0].leaf.mins.y = 0.0f;
        clipMap->cmodels[0].leaf.mins.z = 0.0f;
        clipMap->cmodels[0].leaf.maxs.x = 0.0f;
        clipMap->cmodels[0].leaf.maxs.y = 0.0f;
        clipMap->cmodels[0].leaf.maxs.z = 0.0f;
        clipMap->cmodels[0].leaf.leafBrushNode = 0;
        clipMap->cmodels[0].leaf.cluster = 0;

        clipMap->cmodels[0].info = nullptr; // always set to 0
    }

    void ClipMapLinker::loadXModelCollision(clipMap_t* clipMap)
    {
        // Right now XModels aren't supported
        clipMap->numStaticModels = 0;
        clipMap->staticModelList = nullptr;

        // WIP code left in for future support
        /*
        auto gfxWorldAsset = m_context.LoadDependency<AssetGfxWorld>(bsp->bspName);
        assert(gfxWorldAsset != nullptr);
        GfxWorld* gfxWorld = gfxWorldAsset->Asset();

        clipMap->numStaticModels = gfxWorld->dpvs.smodelCount;
        clipMap->staticModelList = new cStaticModel_s[clipMap->numStaticModels];

        for (unsigned int i = 0; i < clipMap->numStaticModels; i++)
        {
            GfxStaticModelDrawInst* gfxModelDrawInst = &gfxWorld->dpvs.smodelDrawInsts[i];
            GfxStaticModelInst* gfxModelInst = &gfxWorld->dpvs.smodelInsts[i];
            cStaticModel_s* currModel = &clipMap->staticModelList[i];

            memset(&currModel->writable, 0, sizeof(cStaticModelWritable));
            currModel->xmodel = gfxModelDrawInst->model;
            currModel->contents = gfxModelDrawInst->model->contents;
            currModel->origin.x = gfxModelDrawInst->placement.origin.x;
            currModel->origin.y = gfxModelDrawInst->placement.origin.y;
            currModel->origin.z = gfxModelDrawInst->placement.origin.z;

            // TODO: this does not account for model rotation or scale
            currModel->absmin.x = gfxModelInst->mins.x;
            currModel->absmin.y = gfxModelInst->mins.y;
            currModel->absmin.z = gfxModelInst->mins.z;
            currModel->absmax.x = gfxModelInst->maxs.x;
            currModel->absmax.y = gfxModelInst->maxs.y;
            currModel->absmax.z = gfxModelInst->maxs.z;

            BSPUtil::matrixTranspose3x3(gfxModelDrawInst->placement.axis, currModel->invScaledAxis);
            currModel->invScaledAxis[0].x = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[0].x;
            currModel->invScaledAxis[0].y = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[0].y;
            currModel->invScaledAxis[0].z = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[0].z;
            currModel->invScaledAxis[1].x = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[1].x;
            currModel->invScaledAxis[1].y = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[1].y;
            currModel->invScaledAxis[1].z = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[1].z;
            currModel->invScaledAxis[2].x = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[2].x;
            currModel->invScaledAxis[2].y = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[2].y;
            currModel->invScaledAxis[2].z = (1.0f / gfxModelDrawInst->placement.scale) * currModel->invScaledAxis[2].z;
        }
        */
    }

    void ClipMapLinker::addAABBTreeFromLeaf(clipMap_t* clipMap, BSPTree* tree, size_t* out_parentCount, size_t* out_parentStartIndex)
    {
        assert(tree->isLeaf);

        size_t leafObjectCount = tree->leaf->getObjectCount();
        assert(leafObjectCount > 0);
        highestLeafObjectCount = std::max(leafObjectCount, highestLeafObjectCount);

        // BO2 has a maximum limit of 128 children per AABB tree (essentially),
        // so this is fixed by adding multiple parent AABB trees that hold 128 children each
        size_t result = leafObjectCount / BSPGameConstants::MAX_AABB_TREE_CHILDREN;
        size_t remainder = leafObjectCount % BSPGameConstants::MAX_AABB_TREE_CHILDREN;
        size_t parentCount = result;
        if (remainder > 0)
            parentCount++;

        size_t parentAABBArrayIndex = AABBTreeVec.size();
        AABBTreeVec.resize(AABBTreeVec.size() + parentCount);
        size_t unaddedObjectCount = leafObjectCount;
        size_t addedObjectCount = 0;
        for (size_t parentIdx = 0; parentIdx < parentCount; parentIdx++)
        {
            size_t childObjectCount = BSPGameConstants::MAX_AABB_TREE_CHILDREN;
            if (unaddedObjectCount <= BSPGameConstants::MAX_AABB_TREE_CHILDREN)
                childObjectCount = unaddedObjectCount;
            else
                unaddedObjectCount -= BSPGameConstants::MAX_AABB_TREE_CHILDREN;

            // add the parent AABB
            vec3_t parentMins;
            vec3_t parentMaxs;
            for (size_t objectIdx = 0; objectIdx < childObjectCount; objectIdx++)
            {
                int partitionIndex = tree->leaf->getObject(addedObjectCount + objectIdx)->partitionIndex;
                CollisionPartition* partition = &clipMap->partitions[partitionIndex];
                for (int uindIdx = 0; uindIdx < partition->nuinds; uindIdx++)
                {
                    uint16_t uind = clipMap->info.uinds[partition->fuind + uindIdx];
                    vec3_t vert = clipMap->verts[uind];

                    // initalise the parent AABB with the first vertex
                    if (objectIdx == 0 && uindIdx == 0)
                    {
                        parentMins = vert;
                        parentMaxs = vert;
                    }

                    BSPUtil::updateAABBWithPoint(vert, parentMins, parentMaxs);
                }
            }
            size_t childObjectStartIndex = AABBTreeVec.size();

            CollisionAabbTree parentAABB;
            parentAABB.origin = BSPUtil::calcMiddleOfAABB(parentMins, parentMaxs);
            parentAABB.halfSize = BSPUtil::calcHalfSizeOfAABB(parentMins, parentMaxs);
            parentAABB.materialIndex = 0; // always use the first material
            parentAABB.childCount = static_cast<uint16_t>(childObjectCount);
            parentAABB.u.firstChildIndex = static_cast<int>(childObjectStartIndex);
            AABBTreeVec.at(parentAABBArrayIndex + parentIdx) = parentAABB;

            // add child AABBs
            for (size_t objectIdx = 0; objectIdx < childObjectCount; objectIdx++)
            {
                int partitionIndex = tree->leaf->getObject(addedObjectCount + objectIdx)->partitionIndex;
                CollisionPartition* partition = &clipMap->partitions[partitionIndex];
                vec3_t childMins;
                vec3_t childMaxs;
                for (int uindIdx = 0; uindIdx < partition->nuinds; uindIdx++)
                {
                    uint16_t uind = clipMap->info.uinds[partition->fuind + uindIdx];
                    vec3_t vert = clipMap->verts[uind];

                    // initalise the child AABB with the first vertex
                    if (uindIdx == 0)
                    {
                        childMins = vert;
                        childMaxs = vert;
                    }

                    BSPUtil::updateAABBWithPoint(vert, childMins, childMaxs);
                }

                CollisionAabbTree childAABBTree;
                childAABBTree.materialIndex = 0; // always use the first material
                childAABBTree.childCount = 0;
                childAABBTree.u.partitionIndex = partitionIndex;
                childAABBTree.origin = BSPUtil::calcMiddleOfAABB(childMins, childMaxs);
                childAABBTree.halfSize = BSPUtil::calcHalfSizeOfAABB(childMins, childMaxs);
                AABBTreeVec.emplace_back(childAABBTree);
            }

            addedObjectCount += childObjectCount;
        }

        *out_parentCount = parentCount;
        *out_parentStartIndex = parentAABBArrayIndex;
    }

    constexpr vec3_t normalX = {1.0f, 0.0f, 0.0f};
    constexpr vec3_t normalY = {0.0f, 1.0f, 0.0f};
    constexpr vec3_t normalZ = {0.0f, 0.0f, 1.0f};

    // returns the index of the node/leaf parsed by the function
    // Nodes are indexed by their index in the node array
    // Leafs are indexed by (-1 - <leaf index>)
    // See https://developer.valvesoftware.com/wiki/BSP_(Source)
    int16_t ClipMapLinker::loadBSPNode(clipMap_t* clipMap, BSPTree* tree)
    {
        if (tree->isLeaf)
        {
            cLeaf_s leaf;

            leaf.cluster = 0;       // always use cluster 0
            leaf.brushContents = 0; // no brushes used so contents is 0
            leaf.terrainContents = BSPEditableConstants::LEAF_TERRAIN_CONTENTS;

            // unused when leafBrushNode == 0
            leaf.mins.x = 0.0f;
            leaf.mins.y = 0.0f;
            leaf.mins.z = 0.0f;
            leaf.maxs.x = 0.0f;
            leaf.maxs.y = 0.0f;
            leaf.maxs.z = 0.0f;
            leaf.leafBrushNode = 0;

            if (tree->leaf->getObjectCount() > 0)
            {
                size_t parentCount = 0;
                size_t parentStartIndex = 0;
                addAABBTreeFromLeaf(clipMap, tree, &parentCount, &parentStartIndex);
                leaf.collAabbCount = static_cast<uint16_t>(parentCount);
                leaf.firstCollAabbIndex = static_cast<uint16_t>(parentStartIndex);
            }
            else
            {
                leaf.firstCollAabbIndex = 0;
                leaf.collAabbCount = 0;
            }

            uint16_t leafIndex = static_cast<uint16_t>(leafVec.size());
            leafVec.emplace_back(leaf);

            return -1 - leafIndex;
        }
        else
        {
            cplane_s plane;
            plane.dist = tree->node->distance;
            if (tree->node->axis == AXIS_X)
            {
                plane.normal = normalX;
                plane.type = 0;
            }
            else if (tree->node->axis == AXIS_Y)
            {
                plane.normal = normalY;
                plane.type = 1;
            }
            else // tree->node->axis == AXIS_Z
            {
                plane.normal = normalZ;
                plane.type = 2;
            }

            plane.signbits = 0;
            if (plane.normal.x < 0.0f)
                plane.signbits |= 1;
            if (plane.normal.y < 0.0f)
                plane.signbits |= 2;
            if (plane.normal.z < 0.0f)
                plane.signbits |= 4;

            plane.pad[0] = 0;
            plane.pad[1] = 0;

            planeVec.emplace_back(plane);

            // The recursion of adding the children through loadBSPNode means the parent node needs to be added before the chilren are loaded
            size_t nodeIndex = nodeVec.size();
            nodeVec.emplace_back();

            cNode_t node;
            node.plane = nullptr; // initalised after the BSP tree has been loaded
            node.children[0] = loadBSPNode(clipMap, tree->node->front.get());
            node.children[1] = loadBSPNode(clipMap, tree->node->back.get());

            nodeVec.at(nodeIndex) = node;

            return static_cast<uint16_t>(nodeIndex);
        }
    }

    void ClipMapLinker::loadBSPTree(clipMap_t* clipMap, BSPData* bsp)
    {
        vec3_t worldMins;
        vec3_t worldMaxs;
        for (unsigned int vertIdx = 0; vertIdx < clipMap->vertCount; vertIdx++)
        {
            vec3_t vertex = clipMap->verts[vertIdx];
            // initalise AABB with the first vertex
            if (vertIdx == 0)
            {
                worldMins = vertex;
                worldMaxs = vertex;
            }
            BSPUtil::updateAABBWithPoint(vertex, worldMins, worldMaxs);
        }
        std::unique_ptr<BSPTree> tree = std::make_unique<BSPTree>(worldMins.x, worldMins.y, worldMins.z, worldMaxs.x, worldMaxs.y, worldMaxs.z, 0);
        assert(!tree->isLeaf);

        for (int partitionIdx = 0; partitionIdx < clipMap->partitionCount; partitionIdx++)
        {
            vec3_t partitionMins;
            vec3_t partitionMaxs;
            CollisionPartition* partition = &clipMap->partitions[partitionIdx];
            for (int uindIdx = 0; uindIdx < partition->nuinds; uindIdx++)
            {
                uint16_t uind = clipMap->info.uinds[partition->fuind + uindIdx];
                vec3_t vert = clipMap->verts[uind];
                // initalise the AABB with the first vertex
                if (uindIdx == 0)
                {
                    partitionMins = vert;
                    partitionMaxs = vert;
                }
                BSPUtil::updateAABBWithPoint(vert, partitionMins, partitionMaxs);
            }
            std::shared_ptr<BSPObject> currObject =
                std::make_shared<BSPObject>(partitionMins.x, partitionMins.y, partitionMins.z, partitionMaxs.x, partitionMaxs.y, partitionMaxs.z, partitionIdx);
            tree->addObjectToTree(std::move(currObject));
        }

        // load planes, nodes, leafs, and AABB trees
        loadBSPNode(clipMap, tree.get());

        clipMap->info.planeCount = static_cast<int>(planeVec.size());
        clipMap->info.planes = m_memory.Alloc<cplane_s>(planeVec.size());
        memcpy(clipMap->info.planes, planeVec.data(), sizeof(cplane_s) * planeVec.size());

        clipMap->numNodes = static_cast<unsigned int>(nodeVec.size());
        clipMap->nodes = m_memory.Alloc<cNode_t>(nodeVec.size());
        memcpy(clipMap->nodes, nodeVec.data(), sizeof(cNode_t) * nodeVec.size());

        clipMap->numLeafs = static_cast<unsigned int>(leafVec.size());
        clipMap->leafs = m_memory.Alloc<cLeaf_s>(leafVec.size());
        memcpy(clipMap->leafs, leafVec.data(), sizeof(cLeaf_s) * leafVec.size());

        clipMap->aabbTreeCount = static_cast<unsigned int>(AABBTreeVec.size());
        clipMap->aabbTrees = m_memory.Alloc<CollisionAabbTree>(AABBTreeVec.size());
        memcpy(clipMap->aabbTrees, AABBTreeVec.data(), sizeof(CollisionAabbTree) * AABBTreeVec.size());

        // The plane of each node have the same index
        for (size_t nodeIdx = 0; nodeIdx < nodeVec.size(); nodeIdx++)
            clipMap->nodes[nodeIdx].plane = &clipMap->info.planes[nodeIdx];

        con::info("Highest leaf object count: {}", highestLeafObjectCount);
    }

    bool ClipMapLinker::loadPartitions(clipMap_t* clipMap, BSPData* bsp)
    {
        // due to tris using uint16_t as the type for indexing the vert array,
        //  any vertex count over the uint16_t max means the vertices above the uint16_t max can't be indexed
        if (static_cast<unsigned int>(bsp->colWorld.vertices.size()) > BSPGameConstants::MAX_COLLISION_VERTS)
        {
            con::error("ERROR: collision vertex count %i exceeds the maximum number: %i!\n", clipMap->vertCount, BSPGameConstants::MAX_COLLISION_VERTS);
            return false;
        }

        clipMap->vertCount = static_cast<unsigned int>(bsp->colWorld.vertices.size());
        clipMap->verts = m_memory.Alloc<vec3_t>(clipMap->vertCount);
        for (unsigned int vertIdx = 0; vertIdx < clipMap->vertCount; vertIdx++)
            clipMap->verts[vertIdx] = bsp->colWorld.vertices[vertIdx].pos;

        // The clipmap index buffer has a unique index for each vertex in the world, compared to the gfxworld's
        //  index buffer having a unique index for each vertex on a surface. This code converts gfxworld indices to clipmap indices.
        std::vector<uint16_t> triIndexVec;
        for (BSPSurface& surface : bsp->colWorld.surfaces)
        {
            int indexOfFirstIndex = surface.indexOfFirstIndex;
            int indexOfFirstVertex = surface.indexOfFirstVertex;
            for (int indexIdx = 0; indexIdx < surface.triCount * 3; indexIdx++)
            {
                int triIndex = bsp->colWorld.indices[indexOfFirstIndex + indexIdx] + indexOfFirstVertex;
                triIndexVec.emplace_back(triIndex);
            }
        }
        // the reinterpret_cast is used as triIndices is just a pointer to an array of indicies, and static_cast can't safely do the conversion
        clipMap->triCount = static_cast<int>(triIndexVec.size() / 3);
        clipMap->triIndices = reinterpret_cast<uint16_t (*)[3]>(m_memory.Alloc<uint16_t>(triIndexVec.size()));
        memcpy(clipMap->triIndices, &triIndexVec[0], sizeof(uint16_t) * triIndexVec.size());

        // partitions are "containers" for vertices. BSP tree leafs contain a list of these partitions to determine the collision within a leaf.
        std::vector<CollisionPartition> partitionVec;
        std::vector<uint16_t> uniqueIndicesVec;
        for (BSPSurface& surface : bsp->colWorld.surfaces)
        {
            // partitions are made for each triangle, not one for each surface.
            //  one for each surface causes physics bugs, as the entire bounding box is considered solid instead of the surface itself (for some reason).
            //  so a partition is made for each triangle which removes the physics bugs but likely makes the game run slower
            int indexOfFirstTri = surface.indexOfFirstIndex / 3;
            int indexOfFirstVertex = surface.indexOfFirstVertex;
            for (int triIdx = 0; triIdx < surface.triCount; triIdx++)
            {
                CollisionPartition partition;
                partition.triCount = 1;
                partition.firstTri = indexOfFirstTri + triIdx;

                partition.nuinds = 3;
                partition.fuind = static_cast<int>(uniqueIndicesVec.size());

                // All tri indices are unique since there is only one tri per partition
                uint16_t* tri = clipMap->triIndices[partition.firstTri];
                uniqueIndicesVec.emplace_back(tri[0]);
                uniqueIndicesVec.emplace_back(tri[1]);
                uniqueIndicesVec.emplace_back(tri[2]);

                partitionVec.emplace_back(partition);
            }
        }
        clipMap->partitionCount = static_cast<int>(partitionVec.size());
        clipMap->partitions = m_memory.Alloc<CollisionPartition>(clipMap->partitionCount);
        memcpy(clipMap->partitions, partitionVec.data(), sizeof(CollisionPartition) * partitionVec.size());

        clipMap->info.nuinds = static_cast<int>(uniqueIndicesVec.size());
        clipMap->info.uinds = m_memory.Alloc<uint16_t>(uniqueIndicesVec.size());
        memcpy(clipMap->info.uinds, uniqueIndicesVec.data(), sizeof(uint16_t) * uniqueIndicesVec.size());

        return true;

        /*
        // Proper unique index creation code kept for future use
        int totalUindCount = 0;
        std::vector<uint16_t> uindVec;
        for (int i = 0; i < clipMap->partitionCount; i++)
        {
            CollisionPartition* currPartition = &clipMap->partitions[i];
            std::vector<uint16_t> uniqueVertVec;
            for (int k = 0; k < currPartition->triCount; k++)
            {
                uint16_t* tri = clipMap->triIndices[currPartition->firstTri + k];
                for (int l = 0; l < 3; l++)
                {
                    bool isVertexIndexUnique = true;
                    uint16_t vertIndex = tri[l];

                    for (size_t m = 0; m < uniqueVertVec.size(); m++)
                    {
                        if (uniqueVertVec[m] == vertIndex)
                        {
                            isVertexIndexUnique = false;
                            break;
                        }
                    }

                    if (isVertexIndexUnique)
                        uniqueVertVec.emplace_back(vertIndex);
                }
            }

            currPartition->fuind = totalUindCount;
            currPartition->nuinds = (int)uniqueVertVec.size();
            uindVec.insert(uindVec.end(), uniqueVertVec.begin(), uniqueVertVec.end());
            totalUindCount += currPartition->nuinds;
        }
        clipMap->info.nuinds = totalUindCount;
        clipMap->info.uinds = m_memory.Alloc<uint16_t>(totalUindCount);
        memcpy(clipMap->info.uinds, &uindVec[0], sizeof(uint16_t) * totalUindCount);
        */
    }

    bool ClipMapLinker::loadWorldCollision(clipMap_t* clipMap, BSPData* bsp)
    {
        // No support for brushes, only tris right now
        clipMap->info.numBrushSides = 0;
        clipMap->info.brushsides = nullptr;
        clipMap->info.leafbrushNodesCount = 0;
        clipMap->info.leafbrushNodes = nullptr;
        clipMap->info.numLeafBrushes = 0;
        clipMap->info.leafbrushes = nullptr;
        clipMap->info.numBrushVerts = 0;
        clipMap->info.brushVerts = nullptr;
        clipMap->info.numBrushes = 0;
        clipMap->info.brushes = nullptr;
        clipMap->info.brushBounds = nullptr;
        clipMap->info.brushContents = nullptr;

        // load verts, tris, uinds and partitions
        if (!loadPartitions(clipMap, bsp))
            return false;

        loadBSPTree(clipMap, bsp);

        return true;
    }

    clipMap_t* ClipMapLinker::linkClipMap(BSPData* bsp)
    {
        clipMap_t* clipMap = m_memory.Alloc<clipMap_t>();
        clipMap->name = m_memory.Dup(bsp->bspName.c_str());

        clipMap->isInUse = true;
        clipMap->checksum = 0;
        clipMap->pInfo = nullptr;

        std::string mapEntsName = bsp->bspName;
        auto mapEntsAsset = m_context.LoadDependency<AssetMapEnts>(mapEntsName);
        assert(mapEntsAsset != nullptr);
        clipMap->mapEnts = mapEntsAsset->Asset();

        loadBoxData(clipMap);

        loadVisibility(clipMap);

        loadRopesAndConstraints(clipMap);

        loadSubModelCollision(clipMap, bsp);

        loadDynEnts(clipMap);

        loadXModelCollision(clipMap);

        // Clipmap materials define the properties of a material (bullet penetration, no collision, water, etc)
        // Right now there is no way to define properties per material so only one material is used
        clipMap->info.numMaterials = 1;
        clipMap->info.materials = m_memory.Alloc<ClipMaterial>(clipMap->info.numMaterials);
        clipMap->info.materials[0].name = m_memory.Dup(BSPLinkingConstants::MISSING_IMAGE_NAME);
        clipMap->info.materials[0].contentFlags = BSPEditableConstants::MATERIAL_CONTENT_FLAGS;
        clipMap->info.materials[0].surfaceFlags = BSPEditableConstants::MATERIAL_SURFACE_FLAGS;

        // set all edges to walkable
        // might do weird stuff on walls, but from testing doesnt seem to do anything
        int walkableEdgeSize = (3 * clipMap->triCount + 31) / 32 * 4;
        clipMap->triEdgeIsWalkable = new char[walkableEdgeSize];
        memset(clipMap->triEdgeIsWalkable, 1, walkableEdgeSize * sizeof(char));

        if (!loadWorldCollision(clipMap, bsp))
            return nullptr;

        return clipMap;
    }
} // namespace BSP
