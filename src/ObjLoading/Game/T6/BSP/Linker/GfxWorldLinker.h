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
        T6::GfxWorld* linkGfxWorld(BSPData* bsp);

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;

        void loadDrawData(BSPData* projInfo, T6::GfxWorld* gfxWorld);
        bool loadMapSurfaces(BSPData* projInfo, T6::GfxWorld* gfxWorld);
        void loadXModels(BSPData* projInfo, T6::GfxWorld* gfxWorld);
        void cleanGfxWorld(T6::GfxWorld* gfxWorld);
        void loadGfxLights(T6::GfxWorld* gfxWorld);
        void loadLightGrid(T6::GfxWorld* gfxWorld);
        void loadGfxCells(T6::GfxWorld* gfxWorld);
        void loadModels(T6::GfxWorld* gfxWorld);
        bool loadReflectionProbeData(T6::GfxWorld* gfxWorld);
        bool loadLightmapData(T6::GfxWorld* gfxWorld);
        void loadSkyBox(BSPData* projInfo, T6::GfxWorld* gfxWorld);
        void loadDynEntData(T6::GfxWorld* gfxWorld);
        bool loadOutdoors(T6::GfxWorld* gfxWorld);
        void loadSunData(T6::GfxWorld* gfxWorld);
        void loadWorldBounds(T6::GfxWorld* gfxWorld);
    };
} // namespace BSP
