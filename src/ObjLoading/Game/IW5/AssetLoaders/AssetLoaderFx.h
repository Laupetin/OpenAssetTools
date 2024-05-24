#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "Game/IW5/IW5.h"
#include "SearchPath/ISearchPath.h"

namespace IW5
{
    class AssetLoaderFx final : public BasicAssetLoader<ASSET_TYPE_FX, FxEffectDef>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };
} // namespace IW5
