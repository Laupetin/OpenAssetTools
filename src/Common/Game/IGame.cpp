#include "IGame.h"

#include "IW3/GameIW3.h"
#include "IW4/GameIW4.h"
#include "IW5/GameIW5.h"
#include "T5/GameT5.h"
#include "T6/GameT6.h"
#include "Utils/StringUtils.h"

#include <cassert>

IGame* IGame::GetGameById(GameId gameId)
{
    static IGame* games[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::Game(),
        new IW4::Game(),
        new IW5::Game(),
        new T5::Game(),
        new T6::Game(),
    };

    assert(static_cast<unsigned>(gameId) < static_cast<unsigned>(GameId::COUNT));
    auto* result = games[static_cast<unsigned>(gameId)];
    assert(result);

    return result;
}

AbstractGame::AbstractGame(const char* const* assetTypeNames,
                           const asset_type_t assetTypeCount,
                           const char* const* subAssetTypeNames,
                           const asset_type_t subAssetTypeCount)
    : m_asset_type_names(assetTypeNames),
      m_asset_type_count(assetTypeCount),
      m_sub_asset_type_names(subAssetTypeNames),
      m_sub_asset_type_count(subAssetTypeCount)
{
    for (asset_type_t assetType = 0; assetType < assetTypeCount; ++assetType)
    {
        assert(assetTypeNames[assetType] != nullptr);
        AddAssetTypeNameAlias(assetType, assetTypeNames[assetType]);
    }
}

const std::vector<GameLanguagePrefix>& AbstractGame::GetLanguagePrefixes() const
{
    static std::vector<GameLanguagePrefix> prefixes;
    return prefixes;
}

asset_type_t AbstractGame::GetAssetTypeCount() const
{
    return m_asset_type_count;
}

std::optional<const char*> AbstractGame::GetAssetTypeName(const asset_type_t assetType) const
{
    if (assetType < m_asset_type_count)
        return m_asset_type_names[assetType];

    return std::nullopt;
}

std::optional<asset_type_t> AbstractGame::FindAssetTypeByName(const std::string& potentialAssetTypeName) const
{
    std::string lowerCaseName = potentialAssetTypeName;
    utils::MakeStringLowerCase(lowerCaseName);

    const auto existingAssetType = m_asset_type_name_lookup.find(lowerCaseName);
    if (existingAssetType != m_asset_type_name_lookup.end())
        return existingAssetType->second;

    return std::nullopt;
}

asset_type_t AbstractGame::GetSubAssetTypeCount() const
{
    return m_sub_asset_type_count;
}

std::optional<const char*> AbstractGame::GetSubAssetTypeName(const asset_type_t subAssetType) const
{
    if (subAssetType < m_sub_asset_type_count)
        return m_sub_asset_type_names[subAssetType];

    return std::nullopt;
}

void AbstractGame::AddAssetTypeNameAlias(const asset_type_t assetType, const std::string& assetTypeName)
{
    std::string lowerCaseName = assetTypeName;
    utils::MakeStringLowerCase(lowerCaseName);

    m_asset_type_name_lookup.emplace(lowerCaseName, assetType);
}
