#include "ZoneCreationContext.h"

ZoneCreationContext::ZoneCreationContext()
    : m_definition(nullptr),
      m_asset_search_path(nullptr)
{
}

ZoneCreationContext::ZoneCreationContext(ZoneDefinition* definition, ISearchPath* assetSearchPath)
    : m_definition(definition),
      m_asset_search_path(assetSearchPath)
{
}
