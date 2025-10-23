#pragma once

#include "../BSP.h"
#include "Asset/IAssetCreator.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "../BSPCalculation.h"

namespace BSP
{
	class ClipMapLinker
	{
	public:
		ClipMapLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context);
		AssetCreationResult linkClipMap(BSPData* bsp);

	private:
		MemoryManager& m_memory;
		ISearchPath& m_search_path;
		AssetCreationContext& m_context;

		void loadBoxData(clipMap_t* clipMap);
		void loadVisibility(clipMap_t* clipMap);
		void loadDynEnts(clipMap_t* clipMap);
		void loadRopesAndConstraints(clipMap_t* clipMap);
		void loadSubModelCollision(clipMap_t* clipMap, BSPData* bsp);
		void loadXModelCollision(clipMap_t* clipMap);

		bool loadBrushCollision(clipMap_t* clipMap, BSPData* bsp);
		void populateBSPTree(clipMap_t* clipMap, BSPTree* tree);
		bool createPartitions(clipMap_t* clipMap, BSPData* bsp);
	};
}