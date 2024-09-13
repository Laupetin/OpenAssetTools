#pragma once
#include "AssetLoading/BasicAssetLoader.h"
#include "AssetLoading/IAssetLoadingManager.h"
#include "Game/T5/T5.h"
#include "SearchPath/ISearchPath.h"

namespace T5
{
    class AssetLoaderXModel final : public BasicAssetLoader<AssetXModel>
    {
        static std::string GetFileNameForAsset(const std::string& assetName);

    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool
            LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
} // namespace T5
