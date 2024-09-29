#include "ZoneDefinition.h"

ZoneDefinitionAsset::ZoneDefinitionAsset(const asset_type_t type, std::string name, const bool isReference)
    : m_asset_type(type),
      m_asset_name(std::move(name)),
      m_is_reference(isReference)
{
}

ZoneDefinitionProperties::ZoneDefinitionProperties() = default;

void ZoneDefinitionProperties::AddProperty(std::string key, std::string value)
{
    m_properties.emplace(std::move(key), std::move(value));
}

void ZoneDefinitionProperties::Include(const ZoneDefinitionProperties& otherProperties)
{
    for (const auto& property : otherProperties.m_properties)
        AddProperty(property.first, property.second);
}

ZoneDefinition::ZoneDefinition()
    : m_type(ProjectType::NONE),
      m_game(GameId::COUNT)
{
}

void ZoneDefinition::Include(const AssetList& assetListToInclude)
{
    for (const auto& entry : assetListToInclude.m_entries)
        m_assets.emplace_back(entry.m_type, entry.m_name, false);
}

void ZoneDefinition::Include(const ZoneDefinition& definitionToInclude)
{
    m_properties.Include(definitionToInclude.m_properties);

    for (const auto& ignore : definitionToInclude.m_ignores)
        m_ignores.emplace_back(ignore);

    for (const auto& asset : definitionToInclude.m_assets)
        m_assets.emplace_back(asset);
}
