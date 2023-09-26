#pragma once

#include "Game/IW5/IW5.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "SearchPath/ISearchPath.h"

namespace IW5
{
    class AssetLoaderMaterial final : public BasicAssetLoader<ASSET_TYPE_MATERIAL, Material>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };
}
