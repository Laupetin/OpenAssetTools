#pragma once

#include "../BSP.h"
#include "../BSPCalculation.h"
#include "Asset/IAssetCreator.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace BSP
{
    class ClipMapLinker
    {
    public:
        ClipMapLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context);
        T6::clipMap_t* linkClipMap(BSPData* bsp);

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;

        void loadBoxData(T6::clipMap_t* clipMap);
        void loadVisibility(T6::clipMap_t* clipMap);
        void loadDynEnts(T6::clipMap_t* clipMap);
        void loadRopesAndConstraints(T6::clipMap_t* clipMap);
        void loadSubModelCollision(T6::clipMap_t* clipMap, BSPData* bsp);
        void loadXModelCollision(T6::clipMap_t* clipMap);

        std::vector<T6::cplane_s> planeVec;
        std::vector<T6::cNode_t> nodeVec;
        std::vector<T6::cLeaf_s> leafVec;
        std::vector<T6::CollisionAabbTree> AABBTreeVec;
        size_t highestLeafObjectCount = 0;
        void addAABBTreeFromLeaf(T6::clipMap_t* clipMap, BSPTree* tree, size_t* out_parentCount, size_t* out_parentStartIndex);
        int16_t loadBSPNode(T6::clipMap_t* clipMap, BSPTree* tree);
        void loadBSPTree(T6::clipMap_t* clipMap, BSPData* bsp);
        bool loadPartitions(T6::clipMap_t* clipMap, BSPData* bsp);
        bool loadWorldCollision(T6::clipMap_t* clipMap, BSPData* bsp);
    };
} // namespace BSP
