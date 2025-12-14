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

        [[nodiscard]] T6::clipMap_t* LinkClipMap(const BSPData& bsp);

    private:
        void LoadBoxData(T6::clipMap_t& clipMap) const;
        void LoadVisibility(T6::clipMap_t& clipMap) const;
        void LoadDynEnts(T6::clipMap_t& clipMap) const;
        void LoadRopesAndConstraints(T6::clipMap_t& clipMap) const;
        void LoadSubModelCollision(T6::clipMap_t& clipMap, const BSPData& bsp) const;
        void LoadXModelCollision(T6::clipMap_t& clipMap) const;

        void AddAABBTreeFromLeaf(T6::clipMap_t& clipMap, const BSPTree& tree, size_t& outParentCount, size_t& outParentStartIndex);
        int16_t LoadBSPNode(T6::clipMap_t& clipMap, const BSPTree& tree);
        void LoadBSPTree(T6::clipMap_t& clipMap, const BSPData& bsp);
        bool LoadPartitions(T6::clipMap_t& clipMap, const BSPData& bsp) const;
        bool LoadWorldCollision(T6::clipMap_t& clipMap, const BSPData& bsp);

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;

        std::vector<T6::cplane_s> planeVec;
        std::vector<T6::cNode_t> nodeVec;
        std::vector<T6::cLeaf_s> leafVec;
        std::vector<T6::CollisionAabbTree> AABBTreeVec;
        size_t highestLeafObjectCount = 0;
    };
} // namespace BSP
