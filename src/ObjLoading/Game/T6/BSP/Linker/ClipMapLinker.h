#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/BSP/BSP.h"
#include "Game/T6/BSP/BSPCalculation.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace BSP
{
    class ClipMapLinker
    {
    public:
        ClipMapLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context);

        [[nodiscard]] T6::clipMap_t* linkClipMap(const BSPData& bsp);

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;

        void loadBoxData(T6::clipMap_t& clipMap) const;
        void loadVisibility(T6::clipMap_t& clipMap) const;
        void loadDynEnts(T6::clipMap_t& clipMap) const;
        void loadRopesAndConstraints(T6::clipMap_t& clipMap) const;
        void loadSubModelCollision(T6::clipMap_t& clipMap, const BSPData& bsp) const;
        void loadXModelCollision(T6::clipMap_t& clipMap) const;

        std::vector<T6::cplane_s> planeVec;
        std::vector<T6::cNode_t> nodeVec;
        std::vector<T6::cLeaf_s> leafVec;
        std::vector<T6::CollisionAabbTree> AABBTreeVec;
        size_t highestLeafObjectCount = 0;

        void addAABBTreeFromLeaf(T6::clipMap_t& clipMap, const BSPTree& tree, size_t& out_parentCount, size_t& out_parentStartIndex);
        int16_t loadBSPNode(T6::clipMap_t& clipMap, const BSPTree& tree);
        void loadBSPTree(T6::clipMap_t& clipMap, const BSPData& bsp);
        bool loadPartitions(T6::clipMap_t& clipMap, const BSPData& bsp) const;
        bool loadWorldCollision(T6::clipMap_t& clipMap, const BSPData& bsp);
    };
} // namespace BSP
