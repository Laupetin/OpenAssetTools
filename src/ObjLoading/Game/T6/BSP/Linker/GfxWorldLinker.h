#pragma once

#include "../BSP.h"
#include "Asset/IAssetCreator.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace BSP
{
	class GfxWorldLinker
	{
	public:
		GfxWorldLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context);
		AssetCreationResult linkGfxWorld(BSPData* bsp);

	private:
		MemoryManager& m_memory;
		ISearchPath& m_search_path;
		AssetCreationContext& m_context;

		void overwriteDrawData(BSPData* projInfo, GfxWorld* gfxWorld);
		bool overwriteMapSurfaces(BSPData* projInfo, GfxWorld* gfxWorld);
		void overwriteMapSModels(BSPData* projInfo, GfxWorld* gfxWorld);
		void cleanGfxWorld(GfxWorld* gfxWorld);
		void overwriteGfxLights(GfxWorld* gfxWorld);
		void overwriteLightGrid(GfxWorld* gfxWorld);
		void updateGfxCells(GfxWorld* gfxWorld);
		void overwriteModels(GfxWorld* gfxWorld);
		bool updateReflectionProbeData(GfxWorld* gfxWorld);
		bool overwriteLightmapData(GfxWorld* gfxWorld);
		void overwriteSkyBox(BSPData* projInfo, GfxWorld* gfxWorld);
		void updateDynEntData(GfxWorld* gfxWorld);
		bool updateOutdoors(GfxWorld* gfxWorld);
	};
}