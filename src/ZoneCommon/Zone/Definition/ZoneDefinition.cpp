#include "ZoneDefinition.h"

ZoneDefinitionObjContainer::ZoneDefinitionObjContainer(std::string name, const ZoneDefinitionObjContainerType type, const unsigned start)
    : ZoneDefinitionObjContainer(std::move(name), type, start, 0u)
{
}

ZoneDefinitionObjContainer::ZoneDefinitionObjContainer(std::string name, const ZoneDefinitionObjContainerType type, const unsigned start, const unsigned end)
    : m_name(std::move(name)),
      m_type(type),
      m_asset_start(start),
      m_asset_end(end)
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
    : m_game(GameId::COUNT),
      m_map_type(ZoneDefinitionMapType::NONE)
{
}
