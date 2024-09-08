#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"

namespace T6
{
    class AssetLoaderDDL final : public BasicAssetLoader<AssetDDL>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };
} // namespace T6
