#pragma once

#include "Game/IW4/IW4.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "AssetLoading/IAssetLoadingManager.h"
#include "SearchPath/ISearchPath.h"

namespace IW4
{
    class AssetLoaderMenuDef final : public BasicAssetLoader<ASSET_TYPE_MENU, menuDef_t>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
}
