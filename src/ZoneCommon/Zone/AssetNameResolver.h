#pragma once

#include "Game/IGame.h"
#include "Zone/ZoneTypes.h"

#include <optional>
#include <string>
#include <unordered_map>

class AssetNameResolver
{
public:
    AssetNameResolver() = default;
    explicit AssetNameResolver(GameId gameId);

    [[nodiscard]] std::optional<asset_type_t> GetAssetTypeByName(const std::string& assetTypeName) const;

private:
    std::unordered_map<std::string, asset_type_t> m_asset_types_by_name;
};
