#pragma once

#include "Game/IW5/IW5.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "SearchPath/ISearchPath.h"

namespace IW5
{
    class AssetLoaderMenuDef final : public BasicAssetLoader<ASSET_TYPE_MENU, menuDef_t>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };
}
