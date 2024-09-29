#include "AssetNameResolverT5.h"

#include "Game/T5/GameAssetPoolT5.h"
#include "Utils/StringUtils.h"

using namespace T5;

AssetNameResolver::AssetNameResolver()
{
    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        AddAssetTypeName(assetType, *GameAssetPoolT5::AssetTypeNameByType(assetType));
}

GameId AssetNameResolver::GetGameId() const
{
    return GameId::T5;
}

std::optional<const char*> AssetNameResolver::GetAssetTypeName(const asset_type_t assetType) const
{
    return GameAssetPoolT5::AssetTypeNameByType(assetType);
}
