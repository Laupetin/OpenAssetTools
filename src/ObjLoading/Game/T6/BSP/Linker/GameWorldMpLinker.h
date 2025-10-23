#pragma once

#include "../BSP.h"
#include "Asset/IAssetCreator.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace BSP
{
	class GameWorldMpLinker
	{
	public:
		GameWorldMpLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context);
		AssetCreationResult linkGameWorldMp(BSPData* bsp);

	private:
		MemoryManager& m_memory;
		ISearchPath& m_search_path;
		AssetCreationContext& m_context;
	};
}