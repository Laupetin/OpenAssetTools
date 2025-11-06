#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/BSP/BSP.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace BSP
{
    class BSPLinker
    {
    public:
        BSPLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context);
        bool linkBSP(BSPData* bsp);

    private:
        T6::FootstepTableDef* addEmptyFootstepTableAsset(std::string assetName);
        bool addDefaultRequiredAssets(BSPData* bsp);

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;
    };
} // namespace BSP
