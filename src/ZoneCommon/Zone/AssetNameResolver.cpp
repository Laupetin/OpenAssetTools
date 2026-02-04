#include "AssetNameResolver.h"

#include "Utils/StringUtils.h"

#include <cassert>

AssetNameResolver::AssetNameResolver(const GameId gameId)
{
    const auto* game = IGame::GetGameById(gameId);
    const auto assetTypeCount = game->GetAssetTypeCount();

    for (asset_type_t assetType = 0; assetType < assetTypeCount; assetType++)
    {
        auto maybeAssetTypeName = game->GetAssetTypeName(assetType);
        assert(maybeAssetTypeName);
        if (!maybeAssetTypeName)
            continue;

        std::string lowerCaseName(*maybeAssetTypeName);
        utils::MakeStringLowerCase(lowerCaseName);
        m_asset_types_by_name.emplace(lowerCaseName, assetType);
    }
}

std::optional<asset_type_t> AssetNameResolver::GetAssetTypeByName(const std::string& assetTypeName) const
{
    std::string lowerCaseName = assetTypeName;
    utils::MakeStringLowerCase(lowerCaseName);

    const auto existingAssetType = m_asset_types_by_name.find(assetTypeName);
    if (existingAssetType != m_asset_types_by_name.end())
        return existingAssetType->second;

    return std::nullopt;
}
