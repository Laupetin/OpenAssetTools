#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/BSP/BSP.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace BSP
{
    class GfxWorldLinker
    {
    public:
        GfxWorldLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context);
        [[nodiscard]] T6::GfxWorld* linkGfxWorld(const BSPData& bsp) const;

    private:
        void loadDrawData(const BSPData& projInfo, T6::GfxWorld& gfxWorld) const;
        bool loadMapSurfaces(const BSPData& projInfo, T6::GfxWorld& gfxWorld) const;
        void loadXModels(const BSPData& projInfo, T6::GfxWorld& gfxWorld) const;
        void cleanGfxWorld(T6::GfxWorld& gfxWorld) const;
        void loadGfxLights(T6::GfxWorld& gfxWorld) const;
        void loadLightGrid(T6::GfxWorld& gfxWorld) const;
        void loadGfxCells(T6::GfxWorld& gfxWorld) const;
        void loadModels(T6::GfxWorld& gfxWorld) const;
        bool loadReflectionProbeData(T6::GfxWorld& gfxWorld) const;
        bool loadLightmapData(T6::GfxWorld& gfxWorld) const;
        void loadSkyBox(const BSPData& projInfo, T6::GfxWorld& gfxWorld) const;
        void loadDynEntData(T6::GfxWorld& gfxWorld) const;
        bool loadOutdoors(T6::GfxWorld& gfxWorld) const;
        void loadSunData(T6::GfxWorld& gfxWorld) const;
        void loadWorldBounds(T6::GfxWorld& gfxWorld) const;

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;
    };
} // namespace BSP
