#pragma once

#include "Game/IW4/IW4.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "SearchPath/ISearchPath.h"

namespace IW4
{
    class AssetLoaderTechniqueSet final : public BasicAssetLoader<ASSET_TYPE_TECHNIQUE_SET, MaterialTechniqueSet>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };
}
