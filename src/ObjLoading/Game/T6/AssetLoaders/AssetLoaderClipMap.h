#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"

namespace T6
{
    class AssetLoaderClipMapBase : public BasicAssetLoaderWithoutType<clipMap_t>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };

    class AssetLoaderClipMap final : public AssetLoaderClipMapBase
    {
    public:
        [[nodiscard]] asset_type_t GetHandlingAssetType() const override;
    };

    class AssetLoaderClipMapPvs final : public AssetLoaderClipMapBase
    {
    public:
        [[nodiscard]] asset_type_t GetHandlingAssetType() const override;
    };
} // namespace T6
