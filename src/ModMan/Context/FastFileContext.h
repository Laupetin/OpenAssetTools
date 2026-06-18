#pragma once

#include "SearchPath/SearchPaths.h"
#include "Zone/Zone.h"

#include <expected>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

class ContextSearchPath
{
public:
    explicit ContextSearchPath(std::unique_ptr<ISearchPath> searchPath);

    std::unique_ptr<ISearchPath> m_search_path;
    unsigned m_ref_count;
};

class LoadedZone
{
public:
    LoadedZone(std::unique_ptr<Zone> zone, std::string filePath, std::vector<std::string> searchPaths);

    [[nodiscard]] Zone& GetZone();
    [[nodiscard]] const Zone& GetZone() const;

    [[nodiscard]] const std::string& GetFilePath() const;
    [[nodiscard]] const std::vector<std::string>& GetSearchPaths() const;

private:
    std::unique_ptr<Zone> m_zone;
    std::string m_file_path;
    std::vector<std::string> m_search_paths;
};

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

    std::expected<LoadedZone*, std::string> LoadFastFile(const std::string& path);
    std::expected<void, std::string> UnloadZone(const std::string& zoneName);

    ReadAccess<const std::vector<std::unique_ptr<LoadedZone>>> GetLoadedZones();
    ReadAccess<ISearchPath> GetSearchPaths();

private:
    std::vector<std::unique_ptr<LoadedZone>> m_loaded_zones;
    std::shared_mutex m_zone_lock;

    SearchPaths m_search_paths;
    std::unordered_map<std::string, std::unique_ptr<ContextSearchPath>> m_context_search_paths;
    std::shared_mutex m_search_path_lock;
};
