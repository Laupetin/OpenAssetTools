#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"

namespace IW4
{
    class AssetLoaderXModel final : public BasicAssetLoader<AssetXModel>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };
} // namespace IW4
