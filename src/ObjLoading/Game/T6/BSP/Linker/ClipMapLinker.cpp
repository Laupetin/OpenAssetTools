#include "ClipMapLinker.h"
#include "../BSPUtil.h"


namespace
{

}

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
        clipMap->ropes = m_memory.Alloc <rope_t> (clipMap->max_ropes);
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

    void aabbCalcOriginAndHalfSize(vec3_t* mins, vec3_t* maxs, vec3_t* out_origin, vec3_t* out_halfSize)
    {
        // Origin is the midpoint: (min + max) / 2
        vec3_t temp;
        temp.x = mins->x + maxs->x;
        temp.y = mins->y + maxs->y;
        temp.z = mins->z + maxs->z;
        out_origin->x = temp.x * 0.5f;
        out_origin->y = temp.y * 0.5f;
        out_origin->z = temp.z * 0.5f;

        // Half-size is half the difference: (max - min) / 2
        temp.x = maxs->x - mins->x;
        temp.y = maxs->y - mins->y;
        temp.z = maxs->z - mins->z;
        out_halfSize->x = temp.x * 0.5f;
        out_halfSize->y = temp.y * 0.5f;
        out_halfSize->z = temp.z * 0.5f;
    }

    void traverseBSPTreeForCounts(BSPTree* node, size_t* numPlanes, size_t* numNodes, size_t* numLeafs, size_t* numAABBTrees, size_t* maxObjsPerLeaf)
    {
        if (node->isLeaf)
        {
            (*numLeafs)++;
            // there won't be an AABB tree when objectList is empty
            if (node->leaf->getObjectCount() > 0)
            {
                *numAABBTrees += node->leaf->getObjectCount() + 1;

                if (node->leaf->getObjectCount() > *maxObjsPerLeaf)
                    *maxObjsPerLeaf = node->leaf->getObjectCount();
            }
        }
        else
        {
            (*numPlanes)++;
            (*numNodes)++;
            traverseBSPTreeForCounts(node->node->front.get(), numPlanes, numNodes, numLeafs, numAABBTrees, maxObjsPerLeaf);
            traverseBSPTreeForCounts(node->node->back.get(), numPlanes, numNodes, numLeafs, numAABBTrees, maxObjsPerLeaf);
        }
    }

    vec3_t normalX = { 1.0f, 0.0f, 0.0f };
    vec3_t normalY = { 0.0f, 1.0f, 0.0f };
    vec3_t normalZ = { 0.0f, 0.0f, 1.0f };

    int currPlaneCount = 0;
    int currNodeCount = 0;
    int currLeafCount = 0;
    int currAABBCount = 0;

    int addAABBTreeFromLeaf(BSPTree* node, clipMap_t* clipMap)
    {
        assert(node->isLeaf);

        int objectCount = node->leaf->getObjectCount();
        int firstAABBIndex = currAABBCount;
        currAABBCount += objectCount + 1;

        // calculate root AABB node mins and maxs
        // cannot convert mins and maxs coord to BO2 directly as this will result in incorrect mins and maxs
        // so we have to recompute every min and max, not hard just tedious
        int firstPartitionIndex = node->leaf->getObject(0)->partitionIndex;
        auto firstPartition = &clipMap->partitions[firstPartitionIndex];
        uint16_t* firstTri = clipMap->triIndices[firstPartition->firstTri];
        vec3_t* firstVert = &clipMap->verts[firstTri[0]];
        vec3_t aabbMins;
        vec3_t aabbMaxs;
        aabbMins.x = firstVert->x;
        aabbMins.y = firstVert->y;
        aabbMins.z = firstVert->z;
        aabbMaxs.x = firstVert->x;
        aabbMaxs.y = firstVert->y;
        aabbMaxs.z = firstVert->z;
        for (int i = 0; i < objectCount; i++)
        {
            int currPartitionIndex = node->leaf->getObject(i)->partitionIndex;
            auto currPartition = &clipMap->partitions[currPartitionIndex];

            for (int k = 0; k < currPartition->triCount; k++)
            {
                uint16_t* tri = clipMap->triIndices[currPartition->firstTri + k];
                for (int l = 0; l < 3; l++)
                {
                    uint16_t vertIndex = tri[l];
                    vec3_t vertCoord = clipMap->verts[vertIndex];
                    BSPUtil::updateAABBWithpoint(&vertCoord, &aabbMins, &aabbMaxs);
                }
            }
        }
        CollisionAabbTree* rootAABB = &clipMap->aabbTrees[firstAABBIndex];
        aabbCalcOriginAndHalfSize(&aabbMins, &aabbMaxs, &rootAABB->origin, &rootAABB->halfSize);
        rootAABB->materialIndex = 0;
        rootAABB->childCount = objectCount;
        rootAABB->u.firstChildIndex = firstAABBIndex + 1;

        // populate child AABB nodes
        for (int i = 0; i < objectCount; i++)
        {
            CollisionAabbTree* currAabbTree = &clipMap->aabbTrees[rootAABB->u.firstChildIndex + i];
            int currPartitionIndex = node->leaf->getObject(i)->partitionIndex;

            currAabbTree->materialIndex = 0;
            currAabbTree->childCount = 0;
            currAabbTree->u.partitionIndex = currPartitionIndex;

            // calculate partition origin and half size
            CollisionPartition* aabbPartition = &clipMap->partitions[currPartitionIndex];
            uint16_t firstUind = clipMap->info.uinds[aabbPartition->fuind];
            vec3_t* firstVertex = &clipMap->verts[firstUind];
            vec3_t mins;
            vec3_t maxs;
            mins.x = firstVertex->x;
            mins.y = firstVertex->y;
            mins.z = firstVertex->z;
            maxs.x = firstVertex->x;
            maxs.y = firstVertex->y;
            maxs.z = firstVertex->z;
            for (int i = 1; i < aabbPartition->nuinds; i++)
            {
                uint16_t currUind = clipMap->info.uinds[aabbPartition->fuind + i];
                vec3_t* currVertex = &clipMap->verts[currUind];

                BSPUtil::updateAABBWithpoint(currVertex, &mins, &maxs);
            }

            aabbCalcOriginAndHalfSize(&mins, &maxs, &currAabbTree->origin, &currAabbTree->halfSize);
        }

        return firstAABBIndex;
    }

    // returns the index corresponding to the BSPTree* node parsed
    int16_t populateBSPTree_r(clipMap_t* clipMap, BSPTree* node)
    {
        if (node->isLeaf)
        {
            int currLeafIndex = currLeafCount;
            currLeafCount++;
            cLeaf_s* currLeaf = &clipMap->leafs[currLeafIndex];

            currLeaf->cluster = 0;
            currLeaf->brushContents = 0;                       // no brushes used so contents is 0
            currLeaf->terrainContents = BSPEditableConstants::LEAF_TERRAIN_CONTENTS; // clipMap->cmodels[0].leaf.terrainContents takes prescedence

            // unused when leafBrushNode == 0
            currLeaf->mins.x = 0.0f;
            currLeaf->mins.y = 0.0f;
            currLeaf->mins.z = 0.0f;
            currLeaf->maxs.x = 0.0f;
            currLeaf->maxs.y = 0.0f;
            currLeaf->maxs.z = 0.0f;
            currLeaf->leafBrushNode = 0;

            if (node->leaf->getObjectCount() > 0)
            {
                currLeaf->firstCollAabbIndex = addAABBTreeFromLeaf(node, clipMap);
                currLeaf->collAabbCount = 1;
            }
            else
            {
                currLeaf->firstCollAabbIndex = 0;
                currLeaf->collAabbCount = 0;
            }

            return -1 - currLeafIndex;
        }
        else
        {
            cplane_s* currPlane = &clipMap->info.planes[currPlaneCount];
            currPlaneCount++;

            if (node->node->axis == AXIS_X)
            {
                // X is unchanged when going from OGL x -> BO2 x
                currPlane->normal = normalX;

                // converting OGL -> BO2 X coords doesn't change the x coords at all, so
                // the dist stays the same
                currPlane->dist = (float)node->node->distance;
            }
            else
            {
                // converting OGL -> BO2 Z coords negates the z coords and sets it to the y coord.
                // convert the z normal to the y normal, but don't negate it. Negative normals don't do
                // what is expected when the game uses them
                assert(node->node->axis == AXIS_Z);
                currPlane->normal = normalY;

                // converting OGL -> BO2 Z coords negates the z coords and sets it to the y coord.
                // just negate it here as it is just the distance from the orgin along the axis
                currPlane->dist = (float)(-node->node->distance);
            }

            bool foundType = false;
            if (currPlane->normal.x == 1.0f)
            {
                assert(!foundType);
                foundType = true;
                currPlane->type = 0;
            }
            else if (currPlane->normal.y == 1.0f)
            {
                assert(!foundType);
                foundType = true;
                currPlane->type = 1;
            }
            else if (currPlane->normal.z == 1.0f)
            {
                assert(!foundType);
                foundType = true;
                currPlane->type = 2;
            }
            else
                assert(foundType);

            currPlane->signbits = 0;
            if (currPlane->normal.x < 0.0f)
                currPlane->signbits |= 1;
            if (currPlane->normal.y < 0.0f)
                currPlane->signbits |= 2;
            if (currPlane->normal.z < 0.0f)
                currPlane->signbits |= 4;

            currPlane->pad[0] = 0;
            currPlane->pad[1] = 0;

            int currNodeIndex = currNodeCount;
            currNodeCount++;
            cNode_t* currNode = &clipMap->nodes[currNodeIndex];

            currNode->plane = currPlane;
            // Reason for the front and back flip (due to the hacky nature of making the mins and maxs work (see createClipMap)):
            // after converting between OGL and BO2 coords and when and updating the normal from Z -> Y,
            // the normal vector flips and objects behind the plane are now in front, and vise versa
            // so the back node now represents the front, and the front node represents the back.
            // Do the OGL -> Bo2 coord change on paper and it will make sense
            if (currPlane->type == 1)
            {
                currNode->children[1] = populateBSPTree_r(clipMap, node->node->front.get());
                currNode->children[0] = populateBSPTree_r(clipMap, node->node->back.get());
            }
            else
            {
                currNode->children[0] = populateBSPTree_r(clipMap, node->node->front.get());
                currNode->children[1] = populateBSPTree_r(clipMap, node->node->back.get());
            }

            return currNodeIndex;
        }
    }

    void ClipMapLinker::populateBSPTree(clipMap_t* clipMap, BSPTree* tree)
    {
        size_t numPlanes = 0;
        size_t numNodes = 0;
        size_t numLeafs = 0;
        size_t numAABBTrees = 0;
        size_t maxObjsPerLeaf = 0;

        traverseBSPTreeForCounts(tree, &numPlanes, &numNodes, &numLeafs, &numAABBTrees, &maxObjsPerLeaf);

        printf("Max Objects per leaf: %i\n", maxObjsPerLeaf);

        clipMap->info.planeCount = numPlanes;
        clipMap->info.planes = m_memory.Alloc<cplane_s>(clipMap->info.planeCount);
        clipMap->numNodes = numNodes;
        clipMap->nodes = m_memory.Alloc<cNode_t>(clipMap->numNodes);
        // aabb trees: each leaf will have their own AABB tree of the objects within it, and the root aabb node will be the parent of every other aabb node.
        // therefore, each aabb tree will be of size (numObjects + 1) as the tree needs a root aabb node to reference it's children.
        clipMap->aabbTreeCount = numAABBTrees;
        clipMap->aabbTrees = m_memory.Alloc<CollisionAabbTree>(clipMap->aabbTreeCount);

        currPlaneCount = 0;
        currNodeCount = 0;
        currAABBCount = 0;

        // first leaf is always empty
        clipMap->numLeafs = numLeafs + 1;
        clipMap->leafs = m_memory.Alloc<cLeaf_s>(clipMap->numLeafs);
        memset(&clipMap->leafs[0], 0, sizeof(cLeaf_s));
        currLeafCount = 1;

        populateBSPTree_r(clipMap, tree);

        assert(clipMap->info.planeCount == currPlaneCount);
        assert(clipMap->numNodes == currNodeCount);
        assert(clipMap->numLeafs == currLeafCount);
        assert(clipMap->aabbTreeCount == currAABBCount);
    }

    bool ClipMapLinker::createPartitions(clipMap_t* clipMap, BSPData* bsp)
    {
        int collisionVertexCount = bsp->colWorld.vertices.size();
        std::vector<vec3_t> collisionVertVec;
        for (int i = 0; i < collisionVertexCount; i++)
        {
            collisionVertVec.push_back(BSPUtil::convertToBO2Coords(bsp->colWorld.vertices[i].pos));
            //collisionVertVec.push_back(bsp->colWorld.vertices[i].pos);
        }
        clipMap->vertCount = collisionVertexCount;
        clipMap->verts = m_memory.Alloc<vec3_t>(collisionVertexCount);
        memcpy(clipMap->verts, &collisionVertVec[0], sizeof(vec3_t) * collisionVertexCount);

        // due to tris using uint16_t as the type for indexing the vert array,
        // any vertex count over the uint16_t max means the vertices above it can't be indexed
        if (collisionVertexCount > BSPGameConstants::MAX_COLLISION_VERTS)
        {
            printf("ERROR: collision vertex count %i exceeds the maximum number: %i!\n", collisionVertexCount, BSPGameConstants::MAX_COLLISION_VERTS);
            return false;
        }

        std::vector<uint16_t> triIndexVec;
        for (size_t i = 0; i < bsp->colWorld.surfaces.size(); i++)
        {
            BSPSurface* currSurface = &bsp->colWorld.surfaces[i];
            int triCount = currSurface->triCount;

            for (int k = 0; k < triCount * 3; k += 3)
            {
                int firstIndex_Index = currSurface->indexOfFirstIndex;
                int firstVertexIndex = currSurface->indexOfFirstVertex;

                // gfx index bufer starts at 0 for each new mesh, while the clipmap index buffer indexes the entire
                // clipmap verts buffer, so this code updates the indexes to follow that.
                int triIndex0 = bsp->colWorld.indices[firstIndex_Index + (k + 0)] + firstVertexIndex;
                int triIndex1 = bsp->colWorld.indices[firstIndex_Index + (k + 1)] + firstVertexIndex;
                int triIndex2 = bsp->colWorld.indices[firstIndex_Index + (k + 2)] + firstVertexIndex;

                // triangle index ordering is opposite to blenders, so its converted here
                triIndexVec.push_back(triIndex2);
                triIndexVec.push_back(triIndex1);
                triIndexVec.push_back(triIndex0);
            }
        }
        assert(triIndexVec.size() % 3 == 0);

        // the reinterpret_cast is used as triIndices is just a pointer to an array of indicies,
        //  and static_cast can't safely do the conversion
        clipMap->triCount = triIndexVec.size() / 3;
        clipMap->triIndices = reinterpret_cast<uint16_t(*)[3]>(m_memory.Alloc<uint16_t>(triIndexVec.size()));
        memcpy(clipMap->triIndices, &triIndexVec[0], sizeof(uint16_t) * triIndexVec.size());

        // partitions are made for each triangle, not one for each surface.
        // one for each surface causes physics bugs, as the entire bounding box is considered solid instead of the surface itself (for some reason).
        // so a partition is made for each triangle which removes the physics bugs but likely makes the game run slower
        std::vector<CollisionPartition> partitionVec;
        for (size_t i = 0; i < bsp->colWorld.surfaces.size(); i++)
        {
            int triCount = bsp->colWorld.surfaces[i].triCount;
            int firstTriIndex = bsp->colWorld.surfaces[i].indexOfFirstIndex / 3;
            for (int k = 0; k < triCount; k++)
            {
                CollisionPartition newPartition;
                newPartition.nuinds = 0; // initialised later
                newPartition.fuind = 0;  // initialised later
                newPartition.triCount = 1;
                newPartition.firstTri = firstTriIndex;
                firstTriIndex += 1;

                partitionVec.push_back(newPartition);
            }
        }
        clipMap->partitionCount = partitionVec.size();
        clipMap->partitions = m_memory.Alloc<CollisionPartition>(clipMap->partitionCount);
        memcpy(clipMap->partitions, &partitionVec[0], sizeof(CollisionPartition) * clipMap->partitionCount);

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
                        uniqueVertVec.push_back(vertIndex);
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

        return true;
    }

    bool ClipMapLinker::loadBrushCollision(clipMap_t* clipMap, BSPData* bsp)
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

        // clipmap BSP creation must go last as it depends on unids, tris and verts already being populated
        // HACK:
        // the BSP tree creation does not work when BO2's coordinate system is used for mins and maxs.
        // Workaround is to convert every BO2 coordinate to OGL's before it is added into the BSP tree,
        // and then convert them back when it is being parsed into the clipmap. Requires some hacky
        // logic, check populateBSPTree_r and addAABBTreeFromLeaf

        if (!createPartitions(clipMap, bsp))
            return false;

        vec3_t* firstVert = &clipMap->verts[0];
        vec3_t clipMins;
        vec3_t clipMaxs;
        clipMins.x = firstVert->x;
        clipMins.y = firstVert->y;
        clipMins.z = firstVert->z;
        clipMaxs.x = firstVert->x;
        clipMaxs.y = firstVert->y;
        clipMaxs.z = firstVert->z;
        clipMins = BSPUtil::convertFromBO2Coords(clipMins);
        clipMaxs = BSPUtil::convertFromBO2Coords(clipMaxs);
        for (unsigned int i = 1; i < clipMap->vertCount; i++)
        {
            vec3_t vertCoord = BSPUtil::convertFromBO2Coords(clipMap->verts[i]);
            BSPUtil::updateAABBWithpoint(&vertCoord, &clipMins, &clipMaxs);
        }

        BSPTree* tree = new BSPTree(clipMins.x, clipMins.y, clipMins.z, clipMaxs.x, clipMaxs.y, clipMaxs.z, 0);

        assert(!tree->isLeaf);

        for (int i = 0; i < clipMap->partitionCount; i++)
        {
            auto currPartition = &clipMap->partitions[i];

            uint16_t* firstTri = clipMap->triIndices[currPartition->firstTri];
            vec3_t* firstVert = &clipMap->verts[firstTri[0]];
            vec3_t mins;
            vec3_t maxs;
            mins.x = firstVert->x;
            mins.y = firstVert->y;
            mins.z = firstVert->z;
            maxs.x = firstVert->x;
            maxs.y = firstVert->y;
            maxs.z = firstVert->z;
            mins = BSPUtil::convertFromBO2Coords(mins);
            maxs = BSPUtil::convertFromBO2Coords(maxs);
            for (int k = 0; k < currPartition->triCount; k++)
            {
                uint16_t* tri = clipMap->triIndices[currPartition->firstTri + k];
                for (int l = 0; l < 3; l++)
                {
                    uint16_t vertIndex = tri[l];
                    vec3_t vertCoord = BSPUtil::convertFromBO2Coords(clipMap->verts[vertIndex]);
                    BSPUtil::updateAABBWithpoint(&vertCoord, &mins, &maxs);
                }
            }

            std::shared_ptr<BSPObject> currObject = std::make_shared<BSPObject>(mins.x, mins.y, mins.z, maxs.x, maxs.y, maxs.z, i);

            tree->addObjectToTree(std::move(currObject));
        }

        populateBSPTree(clipMap, tree);

        return true;
    }

    AssetCreationResult ClipMapLinker::linkClipMap(BSPData* bsp)
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

        if (!loadBrushCollision(clipMap, bsp))
            return AssetCreationResult::Failure();

        m_context.AddAsset<AssetClipMapPvs>(clipMap->name, clipMap);

        auto clipMapAsset = m_context.AddAsset<AssetClipMap>(clipMap->name, clipMap);
        return AssetCreationResult::Success(clipMapAsset);
    }
}

