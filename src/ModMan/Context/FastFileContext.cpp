#include "FastFileContext.h"

#include "Game/AutoSearchPaths.h"
#include "IObjLoader.h"
#include "SearchPath/IWD.h"
#include "SearchPath/SearchPathFilesystem.h"
#include "Utils/StringUtils.h"
#include "Web/Binds/ZoneBinds.h"
#include "Web/UiCommunication.h"
#include "ZoneLoading.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace
{
    constexpr double MIN_PROGRESS_TO_REPORT = 0.5;

    class LoadingEventProgressReporter : public ProgressCallback
    {
    public:
        explicit LoadingEventProgressReporter(std::string zoneName)
            : m_zone_name(std::move(zoneName)),
              m_last_progress(0)
        {
        }

        void OnProgress(const size_t current, const size_t total) override
        {
            const double percentage = static_cast<double>(current) / static_cast<double>(total) * 100.0;

            if (percentage - m_last_progress >= MIN_PROGRESS_TO_REPORT)
            {
                m_last_progress = percentage;
                ui::NotifyZoneLoadProgress(m_zone_name, percentage);
            }
        }

    private:
        std::string m_zone_name;
        double m_last_progress;
    };

    std::unique_ptr<ISearchPath> CreateSearchPath(const std::string& searchPathStr)
    {
        auto searchPath = std::make_unique<SearchPathFilesystem>(searchPathStr);
        con::debug("Loaded search path \"{}\"", searchPathStr);

        SearchPaths searchPaths;
        bool hasIwds = false;

        std::filesystem::directory_iterator iterator(searchPathStr);
        const auto end = fs::end(iterator);
        for (auto i = fs::begin(iterator); i != end; ++i)
        {
            if (!i->is_regular_file())
                continue;

            auto extension = i->path().extension().string();
            utils::MakeStringLowerCase(extension);
            if (extension == ".iwd")
            {
                std::string iwdPath = i->path().string();
                auto iwd = iwd::LoadFromFile(iwdPath);
                if (iwd)
                {
                    if (!hasIwds)
                    {
                        searchPaths.CommitSearchPath(std::move(searchPath));
                        hasIwds = true;
                    }

                    searchPaths.CommitSearchPath(std::move(iwd));
                    con::debug("Loaded search path \"{}\"", iwdPath);
                }
            }
        }

        if (hasIwds)
            return std::make_unique<SearchPaths>(std::move(searchPaths));

        return searchPath;
    }
} // namespace

ContextSearchPath::ContextSearchPath(std::unique_ptr<ISearchPath> searchPath)
    : m_search_path(std::move(searchPath)),
      m_ref_count(1)
{
}

LoadedZone::LoadedZone(std::unique_ptr<Zone> zone, std::string filePath, std::vector<std::string> searchPaths)
    : m_zone(std::move(zone)),
      m_file_path(std::move(filePath)),
      m_search_paths(std::move(searchPaths))
{
}

Zone& LoadedZone::GetZone()
{
    return *m_zone;
}

const Zone& LoadedZone::GetZone() const
{
    return *m_zone;
}

const std::string& LoadedZone::GetFilePath() const
{
    return m_file_path;
}

const std::vector<std::string>& LoadedZone::GetSearchPaths() const
{
    return m_search_paths;
}

void FastFileContext::Destroy()
{
    // Unload all zones
    m_loaded_zones.clear();
}

std::expected<LoadedZone*, std::string> FastFileContext::LoadFastFile(const std::string& path)
{
    auto zone = ZoneLoading::LoadZone(path, std::make_unique<LoadingEventProgressReporter>(fs::path(path).filename().replace_extension().string()));
    if (!zone)
        return std::unexpected(std::move(zone.error()));

    auto searchPathsForZone = AutoSearchPaths::GetForGame((*zone)->m_game_id)->GetSearchPathsForZonePath(path);
    {
        std::lock_guard lock(m_search_path_lock);
        for (const auto& searchPathStr : searchPathsForZone)
        {
            const auto existingSearchPath = m_context_search_paths.find(searchPathStr);
            if (existingSearchPath == m_context_search_paths.end())
            {
                auto searchPath = CreateSearchPath(searchPathStr);
                m_search_paths.IncludeSearchPath(searchPath.get());
                m_context_search_paths.emplace(searchPathStr, std::make_unique<ContextSearchPath>(std::move(searchPath)));
            }
            else
            {
                existingSearchPath->second->m_ref_count++;
            }
        }
    }

    auto loadedZone = std::make_unique<LoadedZone>(std::move(*zone), path, std::move(searchPathsForZone));

    LoadedZone* loadedZonePtr;
    {
        std::lock_guard lock(m_zone_lock);
        loadedZonePtr = m_loaded_zones.emplace_back(std::move(loadedZone)).get();
    }

    {
        std::shared_lock lock(m_search_path_lock);
        IObjLoader::GetObjLoaderForGame(loadedZonePtr->GetZone().m_game_id)->LoadReferencedContainersForZone(m_search_paths, loadedZonePtr->GetZone());
    }

    ui::NotifyZoneLoaded(*loadedZonePtr);

    return loadedZonePtr;
}

std::expected<void, std::string> FastFileContext::UnloadZone(const std::string& zoneName)
{
    std::unique_ptr<LoadedZone> removedLoadedZone;

    {
        std::lock_guard lock(m_zone_lock);
        const auto existingZone = std::ranges::find_if(m_loaded_zones,
                                                       [&zoneName](const std::unique_ptr<LoadedZone>& loadedZone)
                                                       {
                                                           return loadedZone->GetZone().m_name == zoneName;
                                                       });

        if (existingZone == m_loaded_zones.end())
            return std::unexpected(std::format("No zone with name {} loaded", zoneName));

        removedLoadedZone = std::move(*existingZone);
        m_loaded_zones.erase(existingZone);

        ui::NotifyZoneUnloaded(zoneName);
    }

    assert(removedLoadedZone);

    {
        std::shared_lock lock(m_search_path_lock);
        IObjLoader::GetObjLoaderForGame(removedLoadedZone->GetZone().m_game_id)->UnloadContainersOfZone(removedLoadedZone->GetZone());
    }

    {
        std::lock_guard lock(m_search_path_lock);
        for (const auto& searchPathStr : removedLoadedZone->GetSearchPaths())
        {
            const auto existingSearchPath = m_context_search_paths.find(searchPathStr);
            if (existingSearchPath != m_context_search_paths.end())
            {
                assert(existingSearchPath->second->m_ref_count > 0);
                const auto newRefCount = --existingSearchPath->second->m_ref_count;

                if (newRefCount == 0)
                {
                    m_search_paths.RemoveSearchPath(existingSearchPath->second->m_search_path.get());
                    m_context_search_paths.erase(existingSearchPath);
                    con::debug("Unloaded search path \"{}\"", searchPathStr);
                }
            }
        }
    }

    return {};
}

ReadAccess<const std::vector<std::unique_ptr<LoadedZone>>> FastFileContext::GetLoadedZones()
{
    return ReadAccess<const std::vector<std::unique_ptr<LoadedZone>>>(std::shared_lock(m_zone_lock), m_loaded_zones);
}

ReadAccess<ISearchPath> FastFileContext::GetSearchPaths()
{
    return ReadAccess<ISearchPath>(std::shared_lock(m_search_path_lock), m_search_paths);
}
