#pragma once

#include "Utils/Result.h"
#include "Zone/Zone.h"

#include <memory>
#include <shared_mutex>
#include <vector>

class LoadedZone
{
public:
    std::unique_ptr<Zone> m_zone;
    std::string m_file_path;

    LoadedZone(std::unique_ptr<Zone> zone, std::string filePath);
};

class FastFileContext
{
public:
    void Destroy();

    result::Expected<LoadedZone*, std::string> LoadFastFile(const std::string& path);
    result::Expected<NoResult, std::string> UnloadZone(const std::string& zoneName);

    std::vector<std::unique_ptr<LoadedZone>> m_loaded_zones;
    std::shared_mutex m_zone_lock;
};
