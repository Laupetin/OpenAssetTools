#pragma once

#include "Zone.h"

#include <memory>
#include <string>
#include <vector>

class LoadedZoneInformation
{
public:
    LoadedZoneInformation(std::unique_ptr<Zone> zone, std::string filePath, std::vector<std::string> searchPaths);

    [[nodiscard]] Zone& GetZone();
    [[nodiscard]] const Zone& GetZone() const;

    [[nodiscard]] const std::string& GetFilePath() const;
    [[nodiscard]] const std::vector<std::string>& GetSearchPaths() const;

private:
    std::unique_ptr<Zone> m_zone;
    std::string m_file_path;
    std::vector<std::string> m_search_paths;
};
