#include "ZoneDefinition.h"

ZoneDefinitionEntry::ZoneDefinitionEntry()
    : m_is_reference(false)
{
}

ZoneDefinitionEntry::ZoneDefinitionEntry(std::string type, std::string name, bool isReference)
    : m_asset_type(std::move(type)),
      m_asset_name(std::move(name)),
      m_is_reference(isReference)
{
}

void ZoneDefinition::Include(ZoneDefinition& definitionToInclude)
{
    for(const auto& [key, value] : definitionToInclude.m_metadata)
    {
        if(m_metadata.find(key) == m_metadata.end())
        {
            m_metadata.emplace(std::make_pair(key, value));
        }
    }

    for(const auto& ignore : definitionToInclude.m_ignores)
    {
        m_ignores.emplace_back(ignore);
    }

    for(const auto& asset : definitionToInclude.m_assets)
    {
        m_assets.emplace_back(asset);
    }
}
