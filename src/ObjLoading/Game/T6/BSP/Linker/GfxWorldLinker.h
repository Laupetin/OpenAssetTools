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

        void loadDrawData(BSPData* projInfo, GfxWorld* gfxWorld);
        bool loadMapSurfaces(BSPData* projInfo, GfxWorld* gfxWorld);
        void loadXModels(BSPData* projInfo, GfxWorld* gfxWorld);
        void cleanGfxWorld(GfxWorld* gfxWorld);
        void loadGfxLights(GfxWorld* gfxWorld);
        void loadLightGrid(GfxWorld* gfxWorld);
        void loadGfxCells(GfxWorld* gfxWorld);
        void loadModels(GfxWorld* gfxWorld);
        bool loadReflectionProbeData(GfxWorld* gfxWorld);
        bool loadLightmapData(GfxWorld* gfxWorld);
        void loadSkyBox(BSPData* projInfo, GfxWorld* gfxWorld);
        void loadDynEntData(GfxWorld* gfxWorld);
        bool loadOutdoors(GfxWorld* gfxWorld);
        void loadSunData(GfxWorld* gfxWorld);
        void loadWorldBounds(GfxWorld* gfxWorld);
    };
} // namespace BSP
