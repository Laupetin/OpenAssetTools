#include "ZoneDefinition.h"

ZoneDefinitionEntry::ZoneDefinitionEntry()
    : m_is_reference(false)
{
}

ZoneDefinitionEntry::ZoneDefinitionEntry(std::string type, std::string name, const bool isReference)
    : m_asset_type(std::move(type)),
      m_asset_name(std::move(name)),
      m_is_reference(isReference)
{
}

ZoneMetaDataEntry::ZoneMetaDataEntry() = default;

ZoneMetaDataEntry::ZoneMetaDataEntry(std::string key, std::string value)
    : m_key(std::move(key)),
      m_value(std::move(value))
{
}

void ZoneDefinition::AddMetaData(std::string key, std::string value)
{
    auto metaData = std::make_unique<ZoneMetaDataEntry>(std::move(key), std::move(value));
    auto* metaDataPtr = metaData.get();
    m_metadata.emplace_back(std::move(metaData));
    m_metadata_lookup.emplace(std::make_pair(metaDataPtr->m_key, metaDataPtr));
}

void ZoneDefinition::Include(const AssetList& assetListToInclude)
{
    for (const auto& entry : assetListToInclude.m_entries)
    {
        m_assets.emplace_back(entry.m_type, entry.m_name, false);
    }
}

void ZoneDefinition::Include(const ZoneDefinition& definitionToInclude)
{
    for (const auto& metaData : definitionToInclude.m_metadata)
    {
        AddMetaData(metaData->m_key, metaData->m_value);
    }

    for (const auto& ignore : definitionToInclude.m_ignores)
    {
        m_ignores.emplace_back(ignore);
    }

    for (const auto& asset : definitionToInclude.m_assets)
    {
        m_assets.emplace_back(asset);
    }
}
