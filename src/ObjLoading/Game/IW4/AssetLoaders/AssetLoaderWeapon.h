#pragma once

#include "Game/IW4/IW4.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "SearchPath/ISearchPath.h"

namespace IW4
{
    class AssetLoaderWeapon final : public BasicAssetLoader<ASSET_TYPE_WEAPON, WeaponCompleteDef>
    {
    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
    };
}
