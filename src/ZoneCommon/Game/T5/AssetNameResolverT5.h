#pragma once

#include "Zone/AssetNameResolver.h"

namespace T5
{
    class AssetNameResolver final : public HashMapBasedAssetNameResolver
    {
    public:
        AssetNameResolver();
        [[nodiscard]] GameId GetGameId() const override;
        [[nodiscard]] std::optional<const char*> GetAssetTypeName(asset_type_t assetType) const override;
    };
} // namespace T5
