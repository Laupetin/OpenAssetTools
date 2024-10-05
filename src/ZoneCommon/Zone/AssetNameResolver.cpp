#include "AssetNameResolver.h"

#include "Game/IW3/AssetNameResolverIW3.h"
#include "Game/IW4/AssetNameResolverIW4.h"
#include "Game/IW5/AssetNameResolverIW5.h"
#include "Game/T5/AssetNameResolverT5.h"
#include "Game/T6/AssetNameResolverT6.h"
#include "Utils/StringUtils.h"

#include <cassert>

const IAssetNameResolver* IAssetNameResolver::GetResolverForGame(GameId game)
{
    static const IAssetNameResolver* assetNameResolvers[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::AssetNameResolver(),
        new IW4::AssetNameResolver(),
        new IW5::AssetNameResolver(),
        new T5::AssetNameResolver(),
        new T6::AssetNameResolver(),
    };

    assert(static_cast<unsigned>(game) < static_cast<unsigned>(GameId::COUNT));
    const auto* result = assetNameResolvers[static_cast<unsigned>(game)];
    assert(result);

    return result;
}

void HashMapBasedAssetNameResolver::AddAssetTypeName(asset_type_t assetType, std::string name)
{
    utils::MakeStringLowerCase(name);
    m_asset_types_by_name.emplace(std::move(name), assetType);
}

std::optional<asset_type_t> HashMapBasedAssetNameResolver::GetAssetTypeByName(const std::string& assetTypeName) const
{
    std::string lowerCaseName = assetTypeName;
    utils::MakeStringLowerCase(lowerCaseName);

    const auto existingAssetType = m_asset_types_by_name.find(assetTypeName);
    if (existingAssetType != m_asset_types_by_name.end())
        return existingAssetType->second;

    return std::nullopt;
}
