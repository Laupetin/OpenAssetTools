#pragma once

#include "../BSP.h"
#include "Asset/IAssetCreator.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace BSP
{
    class BSPLinker
    {
    public:
        BSPLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context);
        AssetCreationResult linkBSP(BSPData* bsp);

    private:
        FootstepTableDef* addEmptyFootstepTableAsset(std::string assetName);
        bool addDefaultRequiredAssets(BSPData* bsp);

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;
    };
} // namespace BSP
