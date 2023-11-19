#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"

namespace IW4
{
    class AssetLoaderClipMap : public BasicAssetLoaderWithoutType<clipMap_t>
    {
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };

    class AssetLoaderClipMapSp final : public AssetLoaderClipMap
    {
    public:
        _NODISCARD asset_type_t GetHandlingAssetType() const override;
    };

    class AssetLoaderClipMapMp final : public AssetLoaderClipMap
    {
    public:
        _NODISCARD asset_type_t GetHandlingAssetType() const override;
    };
} // namespace IW4
