#pragma once
#include "AssetLoading/IAssetLoader.h"

namespace IW4
{
    class AssetLoader final : public IAssetLoader
    {
    public:
        _NODISCARD bool SupportsGame(const std::string& gameName) const override;
        _NODISCARD std::unique_ptr<Zone> CreateZoneForDefinition(AssetLoadingContext& context) const override;
    };
}
