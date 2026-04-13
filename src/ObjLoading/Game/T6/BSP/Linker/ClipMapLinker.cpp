#include "ClipMapLinker.h"

#include "../BSPUtil.h"

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

    bool ClipMapLinker::loadXModelCollision(clipMap_t* clipMap, BSPData* bsp)
    {
        // it seems like for players to be able to collide with xmodels, it requires xmodel->collmaps to be valid.
        // A lot of XModels don't implement collmaps (OAT also doesn't generate these collmaps), and
        //  even official maps instead use terrain or brushes to cover where the collision should be.

        clipMap->numStaticModels = bsp->colWorld.xmodels.size();
        clipMap->staticModelList = m_memory.Alloc<cStaticModel_s>(clipMap->numStaticModels);

        for (unsigned int i = 0; i < clipMap->numStaticModels; i++)
        {
            cStaticModel_s* currModel = &clipMap->staticModelList[i];
            BSPXModel& bspModel = bsp->colWorld.xmodels.at(i);

            vec3_t xmodelAxis[3];
            BSPUtil::convertQuaternionToAxis(&bspModel.rotationQuaternion, xmodelAxis);

            auto xModelAsset = m_context.LoadDependency<AssetXModel>(bspModel.name);
            if (xModelAsset == nullptr)
            {
                con::error("Unable to load xmodel asset: \"{}\"", bspModel.name);
                return false;
            }
            else
                currModel->xmodel = (XModel*)xModelAsset->Asset();

            currModel->origin.x = bspModel.origin.x;
            currModel->origin.y = bspModel.origin.y;
            currModel->origin.z = bspModel.origin.z;

            // tracing only checks if this is equal to 0 or not
            currModel->contents = 1;

            if (!xModelAsset->IsReference())
            {
                BSPUtil::calculateXmodelBounds(currModel->xmodel, xmodelAxis, currModel->absmin, currModel->absmax);
                currModel->absmin.x = (currModel->absmin.x * bspModel.scale) + bspModel.origin.x;
                currModel->absmin.y = (currModel->absmin.y * bspModel.scale) + bspModel.origin.y;
                currModel->absmin.z = (currModel->absmin.z * bspModel.scale) + bspModel.origin.z;
                currModel->absmax.x = (currModel->absmax.x * bspModel.scale) + bspModel.origin.x;
                currModel->absmax.y = (currModel->absmax.y * bspModel.scale) + bspModel.origin.y;
                currModel->absmax.z = (currModel->absmax.z * bspModel.scale) + bspModel.origin.z;

                if (currModel->xmodel->numCollmaps == 0)
                    con::warn("Xmodel \"{}\" has no colision data", bspModel.name);
            }
            else
            {
                if (bspModel.areBoundsValid)
                {
                    currModel->absmin = bspModel.mins;
                    currModel->absmax = bspModel.maxs;
                }
                else
                {
                    con::warn("Unable to determine the bounds of xmodel: \"{}\"", bspModel.name);
                    currModel->absmin.x = bspModel.origin.x - 1.0f;
                    currModel->absmin.y = bspModel.origin.y - 1.0f;
                    currModel->absmin.z = bspModel.origin.z - 1.0f;
                    currModel->absmax.x = bspModel.origin.x + 1.0f;
                    currModel->absmax.y = bspModel.origin.y + 1.0f;
                    currModel->absmax.z = bspModel.origin.z + 1.0f;
                }
            }

            BSPUtil::matrixTranspose3x3(xmodelAxis, currModel->invScaledAxis);
            currModel->invScaledAxis[0].x = (1.0f / bspModel.scale) * currModel->invScaledAxis[0].x;
            currModel->invScaledAxis[0].y = (1.0f / bspModel.scale) * currModel->invScaledAxis[0].y;
            currModel->invScaledAxis[0].z = (1.0f / bspModel.scale) * currModel->invScaledAxis[0].z;
            currModel->invScaledAxis[1].x = (1.0f / bspModel.scale) * currModel->invScaledAxis[1].x;
            currModel->invScaledAxis[1].y = (1.0f / bspModel.scale) * currModel->invScaledAxis[1].y;
            currModel->invScaledAxis[1].z = (1.0f / bspModel.scale) * currModel->invScaledAxis[1].z;
            currModel->invScaledAxis[2].x = (1.0f / bspModel.scale) * currModel->invScaledAxis[2].x;
            currModel->invScaledAxis[2].y = (1.0f / bspModel.scale) * currModel->invScaledAxis[2].y;
            currModel->invScaledAxis[2].z = (1.0f / bspModel.scale) * currModel->invScaledAxis[2].z;

            memset(&currModel->writable, 0, sizeof(cStaticModelWritable));
        }

        return true;
    }

    void calculatePartitionAABB(clipMap_t* clipMap, CollisionPartition* partition, vec3_t& out_mins, vec3_t& out_maxs);
    void addAABBTreeFromPartitions(
        clipMap_t* clipMap, std::vector<int>& partitions, size_t* out_parentCount, size_t* out_parentStartIndex, int* out_treeContents);

    void ClipMapLinker::loadSubModelCollision(clipMap_t* clipMap, BSPData* bsp)
    {
        // Submodels are used for the world and map ent collision (triggers, bomb zones, etc)
        clipMap->numSubModels = static_cast<unsigned int>(bsp->models.size() + 1);
        clipMap->cmodels = m_memory.Alloc<cmodel_t>(clipMap->numSubModels);

        // first model is always the world model
        for (unsigned int vertIdx = 0; vertIdx < clipMap->vertCount; vertIdx++)
        {
            vec3_t vertex = clipMap->verts[vertIdx];
            if (vertIdx == 0)
            {
                clipMap->cmodels[0].mins = vertex;
                clipMap->cmodels[0].maxs = vertex;
            }
            else
                BSPUtil::updateAABBWithPoint(vertex, clipMap->cmodels[0].mins, clipMap->cmodels[0].maxs);
        }
        clipMap->cmodels[0].radius = BSPUtil::distBetweenPoints(clipMap->cmodels[0].mins, clipMap->cmodels[0].maxs) / 2;
        // The world sub model has no data apart from the bounds
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
        clipMap->cmodels[0].info = nullptr;

        for (size_t modelIdx = 0; modelIdx < bsp->models.size(); modelIdx++)
        {
            auto clipModel = &clipMap->cmodels[modelIdx + 1];
            auto& bspMpdel = bsp->models.at(modelIdx);

            if (bspMpdel.isGfxModel)
            {
                clipModel->mins.x = 0.0f;
                clipModel->mins.y = 0.0f;
                clipModel->mins.z = 0.0f;
                clipModel->maxs.x = 0.0f;
                clipModel->maxs.y = 0.0f;
                clipModel->maxs.z = 0.0f;
                clipModel->radius = 0.0f;
                clipModel->leaf.brushContents = 0;
                clipModel->leaf.mins.x = 0.0f;
                clipModel->leaf.mins.y = 0.0f;
                clipModel->leaf.mins.z = 0.0f;
                clipModel->leaf.maxs.x = 0.0f;
                clipModel->leaf.maxs.y = 0.0f;
                clipModel->leaf.maxs.z = 0.0f;
                clipModel->leaf.leafBrushNode = 0;
                clipModel->leaf.cluster = 0;
                clipModel->info = nullptr;
                continue;
            }

            std::vector<int> partitionIndexes;
            for (size_t surfIdx = 0; surfIdx < bspMpdel.surfaceCount; surfIdx++)
            {
                ColSurface& surf = collisionSurfaceVec.at(bsp->colWorld.staticSurfaces.size() + bspMpdel.surfaceIndex + surfIdx);
                for (size_t partitionIdx = 0; partitionIdx < surf.partitionCount; partitionIdx++)
                {
                    size_t clipMapPartitionIndex = surf.partitionStartIndex + partitionIdx;
                    partitionIndexes.emplace_back(clipMapPartitionIndex);
                    vec3_t mins;
                    vec3_t maxs;
                    calculatePartitionAABB(clipMap, &clipMap->partitions[clipMapPartitionIndex], mins, maxs);
                    if (surfIdx == 0 && partitionIdx == 0)
                    {
                        clipModel->mins = mins;
                        clipModel->maxs = maxs;
                    }
                    else
                        BSPUtil::updateAABB(mins, maxs, clipModel->mins, clipModel->maxs);
                }
            }
            clipModel->radius = BSPUtil::distBetweenPoints(clipModel->mins, clipModel->maxs) / 2;

            int terrainContents = 0;
            size_t firstCollAabbIndex = 0;
            size_t collAabbCount = 0;
            addAABBTreeFromPartitions(clipMap, partitionIndexes, &collAabbCount, &firstCollAabbIndex, &terrainContents);
            clipModel->leaf.terrainContents = terrainContents;
            clipModel->leaf.firstCollAabbIndex = static_cast<uint16_t>(firstCollAabbIndex);
            clipModel->leaf.collAabbCount = static_cast<uint16_t>(collAabbCount);

            // no brush
            clipModel->leaf.brushContents = 0;
            clipModel->leaf.mins.x = 0.0f;
            clipModel->leaf.mins.y = 0.0f;
            clipModel->leaf.mins.z = 0.0f;
            clipModel->leaf.maxs.x = 0.0f;
            clipModel->leaf.maxs.y = 0.0f;
            clipModel->leaf.maxs.z = 0.0f;
            clipModel->leaf.leafBrushNode = 0;
            clipModel->leaf.cluster = 0;
            clipModel->info = nullptr;
        }
    }

    // out_mins and out_maxs are initialised in the function
    void calculatePartitionAABB(clipMap_t* clipMap, CollisionPartition* partition, vec3_t& out_mins, vec3_t& out_maxs)
    {
        for (int uindIdx = 0; uindIdx < partition->nuinds; uindIdx++)
        {
            uint16_t uind = clipMap->info.uinds[partition->fuind + uindIdx];
            vec3_t vert = clipMap->verts[uind];

            // initalise the AABB with the first vertex
            if (uindIdx == 0)
            {
                out_mins = vert;
                out_maxs = vert;
            }
            else
                BSPUtil::updateAABBWithPoint(vert, out_mins, out_maxs);
        }
    }

    struct uniqueMatData
    {
        size_t materialIndex;
        std::vector<int> partitionIndexes;
    };

    void ClipMapLinker::addAABBTreeFromPartitions(
        clipMap_t* clipMap, std::vector<int>& partitions, size_t* out_parentCount, size_t* out_parentStartIndex, int* out_treeContents)
    {
        size_t partitionCount = partitions.size();
        assert(partitionCount > 0);
        if (partitionCount > highestPartitionCountForAABB)
            highestPartitionCountForAABB = partitionCount;

        std::vector<uniqueMatData> uniqueMaterials;
        for (size_t partArrayIdx = 0; partArrayIdx < partitionCount; partArrayIdx++)
        {
            int partitionIdx = partitions.at(partArrayIdx);
            size_t materialIndex = collisionSurfaceVec.at(partitionToColSurfaceMap.at(partitionIdx)).materialIndex;
            bool foundIdx = false;
            for (auto& uniqueMat : uniqueMaterials)
            {
                if (uniqueMat.materialIndex == materialIndex)
                {
                    uniqueMat.partitionIndexes.emplace_back(partitionIdx);
                    foundIdx = true;
                    break;
                }
            }
            if (!foundIdx)
            {
                uniqueMatData data;
                data.materialIndex = materialIndex;
                data.partitionIndexes.emplace_back(partitionIdx);
                uniqueMaterials.emplace_back(data);
            }
        }

        // BO2 has a maximum limit of 128 children per AABB tree (essentially),
        // so this is fixed by adding multiple parent AABB trees that hold 128 children each
        size_t totalParentCount = 0;
        for (auto& matData : uniqueMaterials)
        {
            size_t objCount = matData.partitionIndexes.size();
            size_t result = objCount / BSPGameConstants::MAX_AABB_TREE_CHILDREN;
            size_t remainder = objCount % BSPGameConstants::MAX_AABB_TREE_CHILDREN;
            if (remainder > 0)
                result++;
            totalParentCount += result;
        }

        // every parent node needs to be contiguous in memory
        size_t parentAABBArrayIndex = AABBTreeVec.size();
        AABBTreeVec.resize(AABBTreeVec.size() + totalParentCount);
        *out_parentCount = totalParentCount;
        *out_parentStartIndex = parentAABBArrayIndex;

        for (auto& matData : uniqueMaterials)
        {
            size_t matPartCount = matData.partitionIndexes.size();
            size_t parentCount = matPartCount / BSPGameConstants::MAX_AABB_TREE_CHILDREN;
            size_t remainder = matPartCount % BSPGameConstants::MAX_AABB_TREE_CHILDREN;
            if (remainder > 0)
                parentCount++;

            size_t unaddedObjectCount = matPartCount;
            size_t addedObjectCount = 0;
            for (size_t parentIdx = 0; parentIdx < parentCount; parentIdx++)
            {
                size_t currChildObjectCount = BSPGameConstants::MAX_AABB_TREE_CHILDREN;
                if (unaddedObjectCount <= BSPGameConstants::MAX_AABB_TREE_CHILDREN)
                    currChildObjectCount = unaddedObjectCount;
                else
                    unaddedObjectCount -= BSPGameConstants::MAX_AABB_TREE_CHILDREN;

                vec3_t parentMins;
                vec3_t parentMaxs;
                size_t childObjectStartIndex = AABBTreeVec.size();
                for (size_t objectIdx = 0; objectIdx < currChildObjectCount; objectIdx++)
                {
                    // create a child AABBTree with the partition and add it to AABBTreeVec
                    int partitionIndex = matData.partitionIndexes.at(addedObjectCount + objectIdx);
                    CollisionPartition* partition = &clipMap->partitions[partitionIndex];
                    vec3_t childMins;
                    vec3_t childMaxs;
                    calculatePartitionAABB(clipMap, partition, childMins, childMaxs);

                    CollisionAabbTree childAABBTree;
                    childAABBTree.materialIndex = static_cast<uint16_t>(matData.materialIndex);
                    childAABBTree.childCount = 0;
                    childAABBTree.u.partitionIndex = partitionIndex;
                    childAABBTree.origin = BSPUtil::calcMiddleOfAABB(childMins, childMaxs);
                    childAABBTree.halfSize = BSPUtil::calcHalfSizeOfAABB(childMins, childMaxs);
                    AABBTreeVec.emplace_back(childAABBTree);

                    // update the parent AABB with the child AABB
                    if (objectIdx == 0)
                    {
                        parentMins = childMins;
                        parentMaxs = childMaxs;
                    }
                    else
                        BSPUtil::updateAABB(childMins, childMaxs, parentMins, parentMaxs);
                }

                CollisionAabbTree parentAABB;
                parentAABB.materialIndex = static_cast<uint16_t>(matData.materialIndex);
                parentAABB.origin = BSPUtil::calcMiddleOfAABB(parentMins, parentMaxs);
                parentAABB.halfSize = BSPUtil::calcHalfSizeOfAABB(parentMins, parentMaxs);
                parentAABB.childCount = static_cast<uint16_t>(currChildObjectCount);
                parentAABB.u.firstChildIndex = static_cast<int>(childObjectStartIndex);
                AABBTreeVec.at(parentAABBArrayIndex + parentIdx) = parentAABB;

                addedObjectCount += currChildObjectCount;
            }

            parentAABBArrayIndex += parentCount;
        }

        *out_treeContents = 0;
        for (auto& matData : uniqueMaterials)
            *out_treeContents |= clipMap->info.materials[matData.materialIndex].contentFlags;
    }

    /*
    void ClipMapLinker::addAABBTreeFromLeaf(clipMap_t* clipMap, BSPTree* tree, size_t* out_parentCount, size_t* out_parentStartIndex, int* out_treeContents)
    {
        assert(tree->isLeaf);
        BSPLeaf* bspLeaf = tree->leaf.get();

        size_t leafObjectCount = bspLeaf->getObjectCount();
        assert(leafObjectCount > 0);
        if (leafObjectCount > highestPartitionCountForAABB)
            highestPartitionCountForAABB = leafObjectCount;

        // the material index of the AABB tree is only checked for the parent node, so each parent has only children with the same material
        std::vector<uniqueMatData> uniqueMaterials;
        for (size_t objIdx = 0; objIdx < leafObjectCount; objIdx++)
        {
            int partitionIdx = bspLeaf->getObject(objIdx)->partitionIndex;
            size_t materialIndex = collisionSurfaceVec.at(partitionToColSurfaceMap.at(partitionIdx)).materialIndex;
            bool foundIdx = false;
            for (auto& uniqueMat : uniqueMaterials)
            {
                if (uniqueMat.materialIndex == materialIndex)
                {
                    uniqueMat.objectIndexes.emplace_back(objIdx);
                    foundIdx = true;
                    break;
                }
            }
            if (!foundIdx)
            {
                uniqueMatData data;
                data.materialIndex = materialIndex;
                data.objectIndexes.emplace_back(objIdx);
                uniqueMaterials.emplace_back(data);
            }
        }

        // BO2 has a maximum limit of 128 children per AABB tree (essentially),
        // so this is fixed by adding multiple parent AABB trees that hold 128 children each
        size_t totalParentCount = 0;
        for (auto& matData : uniqueMaterials)
        {
            size_t objCount = matData.objectIndexes.size();
            size_t result = objCount / BSPGameConstants::MAX_AABB_TREE_CHILDREN;
            size_t remainder = objCount % BSPGameConstants::MAX_AABB_TREE_CHILDREN;
            if (remainder > 0)
                result++;
            totalParentCount += result;
        }

        // every parent node needs to be contiguous in memory
        size_t parentAABBArrayIndex = AABBTreeVec.size();
        AABBTreeVec.resize(AABBTreeVec.size() + totalParentCount);
        *out_parentCount = totalParentCount;
        *out_parentStartIndex = parentAABBArrayIndex;

        for (auto& matData : uniqueMaterials)
        {
            size_t* objIndexes = matData.objectIndexes.data();
            size_t objCount = matData.objectIndexes.size();
            size_t parentCount = objCount / BSPGameConstants::MAX_AABB_TREE_CHILDREN;
            size_t remainder = objCount % BSPGameConstants::MAX_AABB_TREE_CHILDREN;
            if (remainder > 0)
                parentCount++;

            size_t unaddedObjectCount = objCount;
            size_t addedObjectCount = 0;
            for (size_t parentIdx = 0; parentIdx < parentCount; parentIdx++)
            {
                size_t currChildObjectCount = BSPGameConstants::MAX_AABB_TREE_CHILDREN;
                if (unaddedObjectCount <= BSPGameConstants::MAX_AABB_TREE_CHILDREN)
                    currChildObjectCount = unaddedObjectCount;
                else
                    unaddedObjectCount -= BSPGameConstants::MAX_AABB_TREE_CHILDREN;

                vec3_t parentMins;
                vec3_t parentMaxs;
                size_t childObjectStartIndex = AABBTreeVec.size();
                for (size_t objectIdx = 0; objectIdx < currChildObjectCount; objectIdx++)
                {
                    // create a child AABBTree with the partition and add it to AABBTreeVec
                    int partitionIndex = bspLeaf->getObject(objIndexes[addedObjectCount + objectIdx])->partitionIndex;
                    CollisionPartition* partition = &clipMap->partitions[partitionIndex];
                    vec3_t childMins;
                    vec3_t childMaxs;
                    calculatePartitionAABB(clipMap, partition, childMins, childMaxs);

                    CollisionAabbTree childAABBTree;
                    childAABBTree.materialIndex = matData.materialIndex;
                    childAABBTree.childCount = 0;
                    childAABBTree.u.partitionIndex = partitionIndex;
                    childAABBTree.origin = BSPUtil::calcMiddleOfAABB(childMins, childMaxs);
                    childAABBTree.halfSize = BSPUtil::calcHalfSizeOfAABB(childMins, childMaxs);
                    AABBTreeVec.emplace_back(childAABBTree);

                    // update the parent AABB with the child AABB
                    if (objectIdx == 0)
                    {
                        parentMins = childMins;
                        parentMaxs = childMaxs;
                    }
                    else
                        BSPUtil::updateAABB(childMins, childMaxs, parentMins, parentMaxs);
                }

                CollisionAabbTree parentAABB;
                parentAABB.materialIndex = matData.materialIndex;
                parentAABB.origin = BSPUtil::calcMiddleOfAABB(parentMins, parentMaxs);
                parentAABB.halfSize = BSPUtil::calcHalfSizeOfAABB(parentMins, parentMaxs);
                parentAABB.childCount = static_cast<uint16_t>(currChildObjectCount);
                parentAABB.u.firstChildIndex = static_cast<int>(childObjectStartIndex);
                AABBTreeVec.at(parentAABBArrayIndex + parentIdx) = parentAABB;

                addedObjectCount += currChildObjectCount;
            }

            parentAABBArrayIndex += parentCount;
        }

        *out_treeContents = 0;
        for (auto& matData : uniqueMaterials)
            *out_treeContents |= clipMap->info.materials[matData.materialIndex].contentFlags;
    }
    */

    constexpr vec3_t normalX = {1.0f, 0.0f, 0.0f};
    constexpr vec3_t normalY = {0.0f, 1.0f, 0.0f};
    constexpr vec3_t normalZ = {0.0f, 0.0f, 1.0f};

    // returns the index of the node/leaf parsed by the function
    // Nodes are indexed by their index in the node array
    // Leafs are indexed by (-1 - <leaf index>)
    // See https://developer.valvesoftware.com/wiki/BSP_(Source)
    int16_t ClipMapLinker::loadBSPNode(clipMap_t* clipMap, BSPTree* tree, bool isRoot)
    {
        if (tree->isLeaf)
        {
            if (isRoot)
            {
                cplane_s plane;
                plane.dist = 0;
                plane.normal = normalX;
                plane.type = 0;
                plane.signbits = 0;
                plane.pad[0] = 0;
                plane.pad[1] = 0;
                planeVec.emplace_back(plane);

                cNode_t node;
                node.plane = nullptr; // initalised after the BSP tree has been loaded
                node.children[0] = -1;
                node.children[1] = -1;
                nodeVec.emplace_back(node);
            }

            cLeaf_s leaf;

            leaf.cluster = 0; // always use cluster 0

            // no brushes used so data is set to 0
            leaf.brushContents = 0;
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
                int treeContents = 0;
                std::vector<int> partitions;
                for (size_t objIdx = 0; objIdx < tree->leaf->getObjectCount(); objIdx++)
                    partitions.emplace_back(tree->leaf->getObject(objIdx)->partitionIndex);
                addAABBTreeFromPartitions(clipMap, partitions, &parentCount, &parentStartIndex, &treeContents);
                leaf.collAabbCount = static_cast<uint16_t>(parentCount);
                leaf.firstCollAabbIndex = static_cast<uint16_t>(parentStartIndex);
                leaf.terrainContents = treeContents;
            }
            else
            {
                leaf.firstCollAabbIndex = 0;
                leaf.collAabbCount = 0;
                leaf.terrainContents = 0;
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
            node.children[0] = loadBSPNode(clipMap, tree->node->front.get(), false);
            node.children[1] = loadBSPNode(clipMap, tree->node->back.get(), false);

            nodeVec.at(nodeIndex) = node;

            return static_cast<uint16_t>(nodeIndex);
        }
    }

    bool ClipMapLinker::loadBSPTree(clipMap_t* clipMap, BSPData* bsp)
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

        for (size_t surfIdx = 0; surfIdx < bsp->colWorld.staticSurfaces.size(); surfIdx++) // only add static surfaces
        {
            ColSurface& colSurface = collisionSurfaceVec.at(surfIdx);
            for (size_t partitionIdx = 0; partitionIdx < colSurface.partitionCount; partitionIdx++)
            {
                vec3_t partitionMins;
                vec3_t partitionMaxs;
                size_t clipMapPartitionIdx = colSurface.partitionStartIndex + partitionIdx;
                CollisionPartition* partition = &clipMap->partitions[clipMapPartitionIdx];
                calculatePartitionAABB(clipMap, partition, partitionMins, partitionMaxs);
                std::shared_ptr<BSPObject> currObject = std::make_shared<BSPObject>(
                    partitionMins.x, partitionMins.y, partitionMins.z, partitionMaxs.x, partitionMaxs.y, partitionMaxs.z, clipMapPartitionIdx);
                tree->addObjectToTree(std::move(currObject));
            }
        }

        // load planes, nodes, leafs, and AABB trees
        loadBSPNode(clipMap, tree.get(), true);

        clipMap->info.planeCount = static_cast<int>(planeVec.size());
        clipMap->info.planes = m_memory.Alloc<cplane_s>(planeVec.size());
        memcpy(clipMap->info.planes, planeVec.data(), sizeof(cplane_s) * planeVec.size());

        clipMap->numNodes = static_cast<unsigned int>(nodeVec.size());
        clipMap->nodes = m_memory.Alloc<cNode_t>(nodeVec.size());
        memcpy(clipMap->nodes, nodeVec.data(), sizeof(cNode_t) * nodeVec.size());

        clipMap->numLeafs = static_cast<unsigned int>(leafVec.size());
        clipMap->leafs = m_memory.Alloc<cLeaf_s>(leafVec.size());
        memcpy(clipMap->leafs, leafVec.data(), sizeof(cLeaf_s) * leafVec.size());

        // The plane of each node have the same index
        for (size_t nodeIdx = 0; nodeIdx < nodeVec.size(); nodeIdx++)
            clipMap->nodes[nodeIdx].plane = &clipMap->info.planes[nodeIdx];

        con::info("Highest AABB tree partition count: {}", highestPartitionCountForAABB);

        return true;
    }

    bool ClipMapLinker::loadPartitions(clipMap_t* clipMap, BSPData* bsp)
    {
        // due to tris using uint16_t as the type for indexing the vert array,
        //  any vertex count over the uint16_t max means the vertices above the uint16_t max can't be indexed
        if (static_cast<unsigned int>(bsp->colWorld.vertices.size()) > BSPGameConstants::MAX_COLLISION_VERTS)
        {
            con::error(
                "ERROR: collision vertex count {} exceeds the maximum number: {}!\n", bsp->colWorld.vertices.size(), BSPGameConstants::MAX_COLLISION_VERTS);
            return false;
        }

        clipMap->vertCount = static_cast<unsigned int>(bsp->colWorld.vertices.size());
        clipMap->verts = m_memory.Alloc<vec3_t>(clipMap->vertCount);
        for (unsigned int vertIdx = 0; vertIdx < clipMap->vertCount; vertIdx++)
            clipMap->verts[vertIdx] = bsp->colWorld.vertices[vertIdx].pos;

        size_t staticSurfaceCount = bsp->colWorld.staticSurfaces.size();
        size_t scriptSurfaceCount = bsp->colWorld.scriptSurfaces.size();
        size_t totalSurfaceCount = staticSurfaceCount + scriptSurfaceCount;

        // TODO: figure out SV_EntityContact and see how it interacts with partitions

        std::vector<uint16_t> triIndexVec;
        std::vector<CollisionPartition> partitionVec;
        std::vector<uint16_t> uniqueIndicesVec;
        for (size_t surfIdx = 0; surfIdx < totalSurfaceCount; surfIdx++)
        {
            BSPSurface surface;
            if (surfIdx < staticSurfaceCount)
                surface = bsp->colWorld.staticSurfaces.at(surfIdx);
            else
                surface = bsp->colWorld.scriptSurfaces.at(surfIdx - staticSurfaceCount);

            int indexOfFirstIndex = surface.indexOfFirstIndex;
            int indexOfFirstTri = surface.indexOfFirstIndex / 3;
            int indexOfFirstVertex = surface.indexOfFirstVertex;

            ColSurface colSurface;
            colSurface.materialIndex = surface.materialIndex;
            colSurface.partitionCount = surface.triCount;
            colSurface.partitionStartIndex = partitionVec.size();
            collisionSurfaceVec.emplace_back(colSurface);

            for (int triIdx = 0; triIdx < surface.triCount; triIdx++)
            {
                // The clipmap index buffer has a unique index for each vertex in the world, compared to the gfxworld's
                //  index buffer having a unique index for each vertex on a surface. This code converts gfxworld indices to clipmap indices.
                int triIndex0 = bsp->colWorld.indices[indexOfFirstIndex + (triIdx * 3) + 0] + indexOfFirstVertex;
                int triIndex1 = bsp->colWorld.indices[indexOfFirstIndex + (triIdx * 3) + 1] + indexOfFirstVertex;
                int triIndex2 = bsp->colWorld.indices[indexOfFirstIndex + (triIdx * 3) + 2] + indexOfFirstVertex;
                triIndexVec.emplace_back(triIndex0);
                triIndexVec.emplace_back(triIndex1);
                triIndexVec.emplace_back(triIndex2);

                // partitions are "containers" for vertices. BSP tree leafs contain a list of these partitions to determine the collision within a leaf.
                // partitions are made for each triangle, not one for each surface.
                //  one for each surface causes physics bugs, as the entire bounding box is considered solid instead of the surface itself (for some reason).
                //  so a partition is made for each triangle which removes the physics bugs but likely makes the game run slower
                CollisionPartition partition;
                partition.triCount = 1;
                partition.firstTri = indexOfFirstTri + triIdx;

                partition.nuinds = 3;
                partition.fuind = static_cast<int>(uniqueIndicesVec.size());

                // All tri indices are unique since there is only one tri per partition
                int indexOfTriIndex = partition.firstTri * 3;
                uniqueIndicesVec.emplace_back(triIndex0);
                uniqueIndicesVec.emplace_back(triIndex1);
                uniqueIndicesVec.emplace_back(triIndex2);

                partitionVec.emplace_back(partition);
                partitionToColSurfaceMap.emplace_back(collisionSurfaceVec.size() - 1); // -1 as the colSurface has already been added
            }
        }

        clipMap->triCount = static_cast<int>(triIndexVec.size() / 3);
        clipMap->triIndices = reinterpret_cast<uint16_t(*)[3]>(m_memory.Alloc<uint16_t>(triIndexVec.size()));
        memcpy(clipMap->triIndices, &triIndexVec[0], sizeof(uint16_t) * triIndexVec.size());

        clipMap->partitionCount = static_cast<int>(partitionVec.size());
        clipMap->partitions = m_memory.Alloc<CollisionPartition>(clipMap->partitionCount);
        memcpy(clipMap->partitions, partitionVec.data(), sizeof(CollisionPartition) * partitionVec.size());

        clipMap->info.nuinds = static_cast<int>(uniqueIndicesVec.size());
        clipMap->info.uinds = m_memory.Alloc<uint16_t>(uniqueIndicesVec.size());
        memcpy(clipMap->info.uinds, uniqueIndicesVec.data(), sizeof(uint16_t) * uniqueIndicesVec.size());

        return true;

        //// The clipmap index buffer has a unique index for each vertex in the world, compared to the gfxworld's
        ////  index buffer having a unique index for each vertex on a surface. This code converts gfxworld indices to clipmap indices.
        // std::vector<uint16_t> triIndexVec;
        // for (BSPSurface& surface : bsp->colWorld.surfaces)
        //{
        //     int indexOfFirstIndex = surface.indexOfFirstIndex;
        //     int indexOfFirstVertex = surface.indexOfFirstVertex;
        //     for (int indexIdx = 0; indexIdx < surface.triCount * 3; indexIdx++)
        //     {
        //         int triIndex = bsp->colWorld.indices[indexOfFirstIndex + indexIdx] + indexOfFirstVertex;
        //         triIndexVec.emplace_back(triIndex);
        //     }
        // }
        //// the reinterpret_cast is used as triIndices is just a pointer to an array of indicies, and static_cast can't safely do the conversion
        // clipMap->triCount = static_cast<int>(triIndexVec.size() / 3);
        // clipMap->triIndices = reinterpret_cast<uint16_t(*)[3]>(m_memory.Alloc<uint16_t>(triIndexVec.size()));
        // memcpy(clipMap->triIndices, &triIndexVec[0], sizeof(uint16_t) * triIndexVec.size());
        //
        //// partitions are "containers" for vertices. BSP tree leafs contain a list of these partitions to determine the collision within a leaf.
        // std::vector<CollisionPartition> partitionVec;
        // std::vector<uint16_t> uniqueIndicesVec;
        // for (size_t surfIdx = 0; surfIdx < bsp->colWorld.surfaces.size(); surfIdx++)
        //{
        //     BSPSurface& surface = bsp->colWorld.surfaces[surfIdx];
        //
        //     // partitions are made for each triangle, not one for each surface.
        //     //  one for each surface causes physics bugs, as the entire bounding box is considered solid instead of the surface itself (for some reason).
        //     //  so a partition is made for each triangle which removes the physics bugs but likely makes the game run slower
        //     int indexOfFirstTri = surface.indexOfFirstIndex / 3;
        //     int indexOfFirstVertex = surface.indexOfFirstVertex;
        //     for (int triIdx = 0; triIdx < surface.triCount; triIdx++)
        //     {
        //         CollisionPartition partition;
        //         partition.triCount = 1;
        //         partition.firstTri = indexOfFirstTri + triIdx;
        //
        //         partition.nuinds = 3;
        //         partition.fuind = static_cast<int>(uniqueIndicesVec.size());
        //
        //         // All tri indices are unique since there is only one tri per partition
        //         uint16_t* tri = clipMap->triIndices[partition.firstTri];
        //         uniqueIndicesVec.emplace_back(tri[0]);
        //         uniqueIndicesVec.emplace_back(tri[1]);
        //         uniqueIndicesVec.emplace_back(tri[2]);
        //
        //         partitionVec.emplace_back(partition);
        //
        //         partitionToMaterialMap.emplace_back(surface.materialIndex);
        //     }
        // }
        // clipMap->partitionCount = static_cast<int>(partitionVec.size());
        // clipMap->partitions = m_memory.Alloc<CollisionPartition>(clipMap->partitionCount);
        // memcpy(clipMap->partitions, partitionVec.data(), sizeof(CollisionPartition) * partitionVec.size());
        //
        // clipMap->info.nuinds = static_cast<int>(uniqueIndicesVec.size());
        // clipMap->info.uinds = m_memory.Alloc<uint16_t>(uniqueIndicesVec.size());
        // memcpy(clipMap->info.uinds, uniqueIndicesVec.data(), sizeof(uint16_t) * uniqueIndicesVec.size());

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

        if (!loadBSPTree(clipMap, bsp))
            return false;

        return true;
    }

    bool ClipMapLinker::loadMaterials(clipMap_t* clipMap, BSPData* bsp)
    {
        // Clipmap materials define the properties of a material (bullet penetration, no collision, water, etc)

        if (bsp->colWorld.materials.size() > UINT16_MAX)
        {
            con::error("Collision map exceeds 0xFFFF materials");
            return false;
        }

        clipMap->info.numMaterials = static_cast<unsigned int>(bsp->colWorld.materials.size());
        clipMap->info.materials = m_memory.Alloc<ClipMaterial>(clipMap->info.numMaterials);
        for (size_t matIdx = 0; matIdx < bsp->colWorld.materials.size(); matIdx++)
        {
            ClipMaterial* clipMat = &clipMap->info.materials[matIdx];
            BSPMaterial bspMat = bsp->colWorld.materials.at(matIdx);

            clipMat->name = m_memory.Dup(bspMat.materialName.c_str());
            clipMat->contentFlags = bspMat.contentFlags;
            clipMat->surfaceFlags = bspMat.surfaceFlags;
        }

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

        loadDynEnts(clipMap);

        if (!loadXModelCollision(clipMap, bsp))
            return nullptr;

        if (!loadMaterials(clipMap, bsp))
            return nullptr;

        if (!loadWorldCollision(clipMap, bsp)) // requires materials
            return nullptr;

        loadSubModelCollision(clipMap, bsp); // requires tri verts

        // set all edges to walkable
        // might do weird stuff on walls, but from testing doesnt seem to do anything
        int walkableEdgeSize = (3 * clipMap->triCount + 31) / 32 * 4;
        clipMap->triEdgeIsWalkable = m_memory.Alloc<char>(walkableEdgeSize);
        memset(clipMap->triEdgeIsWalkable, 1, walkableEdgeSize * sizeof(char));

        // multiple functions add to AABBTreeVec, so it is added to the clipmap last
        clipMap->aabbTreeCount = static_cast<unsigned int>(AABBTreeVec.size());
        clipMap->aabbTrees = m_memory.Alloc<CollisionAabbTree>(AABBTreeVec.size());
        memcpy(clipMap->aabbTrees, AABBTreeVec.data(), sizeof(CollisionAabbTree) * AABBTreeVec.size());

        return clipMap;
    }
} // namespace BSP
