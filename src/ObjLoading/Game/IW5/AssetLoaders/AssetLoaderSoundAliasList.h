#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "Game/IW5/IW5.h"
#include "SearchPath/ISearchPath.h"

namespace IW5
{
    class AssetLoaderSoundAliasList final : public BasicAssetLoader<ASSET_TYPE_SOUND, snd_alias_list_t>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };
} // namespace IW5
