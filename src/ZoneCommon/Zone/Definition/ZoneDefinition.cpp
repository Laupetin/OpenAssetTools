#include "ZoneDefinition.h"

ZoneDefinitionObjContainer::ZoneDefinitionObjContainer(std::string name, const ZoneDefinitionObjContainerType type, const unsigned start)
    : m_name(std::move(name)),
      m_type(type),
      m_asset_start(start),
      m_asset_end(0u)
{
}

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
    : m_game(GameId::COUNT)
{
}
