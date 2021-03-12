#include "AssetLoadingContext.h"

AssetLoadingContext::AssetLoadingContext()
    : m_asset_search_path(nullptr)
{
}

AssetLoadingContext::AssetLoadingContext(std::string zoneName, ISearchPath* assetSearchPath)
    : m_asset_search_path(assetSearchPath),
      m_zone_name(std::move(zoneName))
{
}
