#include "LoadedZoneInformation.h"

LoadedZoneInformation::LoadedZoneInformation(std::unique_ptr<Zone> zone, std::string filePath, std::vector<std::string> searchPaths)
    : m_zone(std::move(zone)),
      m_file_path(std::move(filePath)),
      m_search_paths(std::move(searchPaths))
{
}

Zone& LoadedZoneInformation::GetZone()
{
    return *m_zone;
}

const Zone& LoadedZoneInformation::GetZone() const
{
    return *m_zone;
}

const std::string& LoadedZoneInformation::GetFilePath() const
{
    return m_file_path;
}

const std::vector<std::string>& LoadedZoneInformation::GetSearchPaths() const
{
    return m_search_paths;
}
