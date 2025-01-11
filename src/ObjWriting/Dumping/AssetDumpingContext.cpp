#include "AssetDumpingContext.h"

#include <filesystem>
#include <format>
#include <fstream>

AssetDumpingContext::AssetDumpingContext(const Zone& zone, const std::string& basePath, IOutputPath& outputPath, ISearchPath& objSearchPath)
    : m_zone(zone),
      m_base_path(basePath),
      m_output_path(outputPath),
      m_obj_search_path(objSearchPath)
{
}

std::unique_ptr<std::ostream> AssetDumpingContext::OpenAssetFile(const std::string& fileName) const
{
    return m_output_path.Open(fileName);
}
