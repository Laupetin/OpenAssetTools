#include "ZoneCreationContext.h"

ZoneCreationContext::ZoneCreationContext()
    : m_asset_search_path(nullptr),
      m_definition(nullptr)
{
}

ZoneCreationContext::ZoneCreationContext(ISearchPath* assetSearchPath, ZoneDefinition* definition)
    : m_asset_search_path(assetSearchPath),
      m_definition(definition)
{
}
