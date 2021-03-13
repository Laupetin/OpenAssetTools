#include "ZoneCreationContext.h"

ZoneCreationContext::ZoneCreationContext()
    : m_asset_search_path(nullptr),
      m_definition(nullptr)
{
}

ZoneCreationContext::ZoneCreationContext(std::string zoneName, ISearchPath* assetSearchPath, ZoneDefinition* definition)
    : m_zone_name(std::move(zoneName)),
      m_asset_search_path(assetSearchPath),
      m_definition(definition)
{
}
