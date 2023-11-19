#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"

namespace IW4
{
    class AssetLoaderGfxLightDef final : public BasicAssetLoader<ASSET_TYPE_LIGHT_DEF, GfxLightDef>
    {
        static constexpr auto MAX_IMAGE_NAME_SIZE = 0x800;

        static std::string GetAssetFilename(const std::string& assetName);

    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool
            LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
} // namespace IW4
