#include "ZoneCreationContext.h"

ZoneCreationContext::ZoneCreationContext()
    : m_asset_search_path(nullptr)
{
}

ZoneCreationContext::ZoneCreationContext(std::string zoneName, ISearchPath* assetSearchPath)
    : m_asset_search_path(assetSearchPath),
      m_zone_name(std::move(zoneName))
{
}
