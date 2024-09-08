#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"

namespace T6
{
    class AssetLoaderTechniqueSet final : public BasicAssetLoader<AssetTechniqueSet>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };
} // namespace T6
