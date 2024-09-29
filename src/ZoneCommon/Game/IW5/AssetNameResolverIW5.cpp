#include "AssetNameResolverIW5.h"

#include "Game/IW5/GameAssetPoolIW5.h"
#include "Utils/StringUtils.h"

using namespace IW5;

AssetNameResolver::AssetNameResolver()
{
    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        AddAssetTypeName(assetType, *GameAssetPoolIW5::AssetTypeNameByType(assetType));
}

GameId AssetNameResolver::GetGameId() const
{
    return GameId::IW5;
}

std::optional<const char*> AssetNameResolver::GetAssetTypeName(const asset_type_t assetType) const
{
    return GameAssetPoolIW5::AssetTypeNameByType(assetType);
}
