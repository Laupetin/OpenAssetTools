#include "AssetNameResolverT6.h"

#include "Game/T6/GameAssetPoolT6.h"
#include "Utils/StringUtils.h"

using namespace T6;

AssetNameResolver::AssetNameResolver()
{
    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        AddAssetTypeName(assetType, *GameAssetPoolT6::AssetTypeNameByType(assetType));

    AddAssetTypeName(ASSET_TYPE_CUSTOM_MAP, "custom_map");
}

GameId AssetNameResolver::GetGameId() const
{
    return GameId::T6;
}

std::optional<const char*> AssetNameResolver::GetAssetTypeName(const asset_type_t assetType) const
{
    return GameAssetPoolT6::AssetTypeNameByType(assetType);
}
