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

        [[nodiscard]] bool LinkBSP(const BSPData& bsp) const;

    private:
        void AddEmptyFootstepTableAsset(const std::string& assetName) const;
        [[nodiscard]] bool AddDefaultRequiredAssets(const BSPData& bsp) const;

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;
    };
} // namespace BSP
