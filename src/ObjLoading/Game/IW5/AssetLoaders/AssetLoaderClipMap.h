#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "Game/IW5/IW5.h"
#include "SearchPath/ISearchPath.h"

namespace IW5
{
    class AssetLoaderClipMap : public BasicAssetLoader<ASSET_TYPE_CLIPMAP, clipMap_t>
    {
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };
} // namespace IW5
