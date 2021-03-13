#pragma once
#include "Game/IW4/IW4.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "AssetLoading/IAssetLoadingManager.h"
#include "SearchPath/ISearchPath.h"

namespace IW4
{
    class AssetLoaderRawFile final : public BasicAssetLoader<ASSET_TYPE_RAWFILE, RawFile>
    {
    public:
        _NODISCARD bool CanLoadFromRaw() const override;
        bool LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager) const override;
    };
}
