#include "AssetNameResolverIW3.h"

#include "Game/IW3/GameAssetPoolIW3.h"
#include "Utils/StringUtils.h"

using namespace IW3;

AssetNameResolver::AssetNameResolver()
{
    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        AddAssetTypeName(assetType, *GameAssetPoolIW3::AssetTypeNameByType(assetType));
}

GameId AssetNameResolver::GetGameId() const
{
    return GameId::IW3;
}

std::optional<const char*> AssetNameResolver::GetAssetTypeName(const asset_type_t assetType) const
{
    return GameAssetPoolIW3::AssetTypeNameByType(assetType);
}
