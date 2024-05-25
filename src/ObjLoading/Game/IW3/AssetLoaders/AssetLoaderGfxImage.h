#pragma once
#include "AssetLoading/BasicAssetLoader.h"
#include "AssetLoading/IAssetLoadingManager.h"
#include "Game/IW3/IW3.h"
#include "SearchPath/ISearchPath.h"

namespace IW3
{
    class AssetLoaderGfxImage final : public BasicAssetLoader<AssetImage>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool
            LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
} // namespace IW3
