#include "AssetNameResolverIW4.h"

#include "Game/IW4/GameAssetPoolIW4.h"
#include "Utils/StringUtils.h"

using namespace IW4;

AssetNameResolver::AssetNameResolver()
{
    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        AddAssetTypeName(assetType, *GameAssetPoolIW4::AssetTypeNameByType(assetType));
}

GameId AssetNameResolver::GetGameId() const
{
    return GameId::IW4;
}

std::optional<const char*> AssetNameResolver::GetAssetTypeName(const asset_type_t assetType) const
{
    return GameAssetPoolIW4::AssetTypeNameByType(assetType);
}
