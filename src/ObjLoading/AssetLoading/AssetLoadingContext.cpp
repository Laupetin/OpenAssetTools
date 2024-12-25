#include "AssetLoadingContext.h"

AssetLoadingContext::AssetLoadingContext(Zone& zone, ISearchPath& rawSearchPath, std::vector<Gdt*> gdtFiles)
    : m_zone(zone),
      m_raw_search_path(rawSearchPath)
{
}
