#pragma once

#include "../BSP.h"
#include "../BSPCalculation.h"
#include "Asset/IAssetCreator.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace BSP
{
    struct ColSurface
    {
        size_t materialIndex;
        size_t partitionCount;
        size_t partitionStartIndex;
    };

    class ClipMapLinker
    {
    public:
        ClipMapLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context);
        clipMap_t* linkClipMap(BSPData* bsp);

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;

        void loadBoxData(clipMap_t* clipMap);
        void loadVisibility(clipMap_t* clipMap);
        void loadDynEnts(clipMap_t* clipMap);
        void loadRopesAndConstraints(clipMap_t* clipMap);
        void loadSubModelCollision(clipMap_t* clipMap, BSPData* bsp);
        bool loadXModelCollision(clipMap_t* clipMap, BSPData* bsp);

        std::vector<cplane_s> planeVec;
        std::vector<cNode_t> nodeVec;
        std::vector<cLeaf_s> leafVec;
        std::vector<CollisionAabbTree> AABBTreeVec;
        size_t highestPartitionCountForAABB = 0;

        std::vector<ColSurface> collisionSurfaceVec;
        std::vector<size_t> partitionToColSurfaceMap;

        void addAABBTreeFromPartitions(
            clipMap_t* clipMap, std::vector<int>& partitions, size_t* out_parentCount, size_t* out_parentStartIndex, int* out_treeContents);
        int16_t loadBSPNode(clipMap_t* clipMap, BSPTree* tree, bool isRoot);
        bool loadBSPTree(clipMap_t* clipMap, BSPData* bsp);
        bool loadPartitions(clipMap_t* clipMap, BSPData* bsp);
        bool loadWorldCollision(clipMap_t* clipMap, BSPData* bsp);
        bool loadMaterials(clipMap_t* clipMap, BSPData* bsp);
    };
} // namespace BSP
