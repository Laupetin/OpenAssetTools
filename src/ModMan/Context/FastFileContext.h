#pragma once

#include "SearchPath/SharedSearchPaths.h"
#include "Zone/LoadedZoneInformation.h"
#include "Zone/Zone.h"

#include <expected>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

template<class T> class ReadAccess
{
public:
    ReadAccess(std::shared_lock<std::shared_mutex> lock, T& data)
        : m_read_lock(std::move(lock)),
          m_data(data)
    {
    }

    [[nodiscard]] T& Data() const
    {
        return m_data;
    }

private:
    std::shared_lock<std::shared_mutex> m_read_lock;
    T& m_data;
};

class FastFileContext
{
public:
    void Destroy();

    std::expected<LoadedZoneInformation*, std::string> LoadFastFile(const std::string& path);
    std::expected<void, std::string> UnloadZone(const std::string& zoneName);

    ReadAccess<const std::vector<std::unique_ptr<LoadedZoneInformation>>> GetLoadedZones();
    ReadAccess<ISearchPath> GetSearchPaths();

private:
    std::vector<std::unique_ptr<LoadedZoneInformation>> m_loaded_zones;
    std::shared_mutex m_zone_lock;

    SharedSearchPaths m_shared_search_paths;
    std::shared_mutex m_search_path_lock;
};
