#include "ZoneCreationContext.h"

namespace fs = std::filesystem;

ZoneCreationContext::ZoneCreationContext()
    : m_definition(nullptr),
      m_asset_search_path(nullptr)
{
}

ZoneCreationContext::ZoneCreationContext(ZoneDefinition* definition, ISearchPath* assetSearchPath, fs::path outDir, fs::path cacheDir)
    : m_definition(definition),
      m_asset_search_path(assetSearchPath),
      m_out_dir(std::move(outDir)),
      m_cache_dir(std::move(cacheDir))
{
}
