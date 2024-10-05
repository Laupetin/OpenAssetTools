#pragma once

#include "Zone/AssetNameResolver.h"

namespace IW3
{
    class AssetNameResolver final : public HashMapBasedAssetNameResolver
    {
    public:
        AssetNameResolver();
        [[nodiscard]] GameId GetGameId() const override;
        [[nodiscard]] std::optional<const char*> GetAssetTypeName(asset_type_t assetType) const override;
    };
} // namespace IW3
