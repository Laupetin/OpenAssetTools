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

        [[nodiscard]] T6::GfxWorld* LinkGfxWorld(const BSPData& bsp) const;

    private:
        void LoadDrawData(const BSPData& projInfo, T6::GfxWorld& gfxWorld) const;
        bool LoadMapSurfaces(const BSPData& projInfo, T6::GfxWorld& gfxWorld) const;
        void LoadXModels(const BSPData& projInfo, T6::GfxWorld& gfxWorld) const;
        void CleanGfxWorld(T6::GfxWorld& gfxWorld) const;
        void LoadGfxLights(T6::GfxWorld& gfxWorld) const;
        void LoadLightGrid(T6::GfxWorld& gfxWorld) const;
        void LoadGfxCells(T6::GfxWorld& gfxWorld) const;
        void LoadModels(T6::GfxWorld& gfxWorld) const;
        bool LoadReflectionProbeData(T6::GfxWorld& gfxWorld) const;
        bool LoadLightmapData(T6::GfxWorld& gfxWorld) const;
        void LoadSkyBox(const BSPData& projInfo, T6::GfxWorld& gfxWorld) const;
        void LoadDynEntData(T6::GfxWorld& gfxWorld) const;
        bool LoadOutdoors(T6::GfxWorld& gfxWorld) const;
        void LoadSunData(T6::GfxWorld& gfxWorld) const;
        void LoadWorldBounds(T6::GfxWorld& gfxWorld) const;

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;
    };
} // namespace BSP
