#pragma once
#include "AssetLoading/BasicAssetLoader.h"
#include "AssetLoading/IAssetLoadingManager.h"
#include "Game/IW5/IW5.h"
#include "SearchPath/ISearchPath.h"

namespace IW5
{
    class AssetLoaderWeaponAttachment final : public BasicAssetLoader<AssetAttachment>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool
            LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
} // namespace IW5
