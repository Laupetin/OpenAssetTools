#include "FastFileContext.h"

#include "Game/AutoSearchPaths.h"
#include "IObjLoader.h"
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
} // namespace

void FastFileContext::Destroy()
{
    // Unload all zones
    m_loaded_zones.clear();
}

std::expected<LoadedZoneInformation*, std::string> FastFileContext::LoadFastFile(const std::string& path)
{
    auto zone = ZoneLoading::LoadZone(path, std::make_unique<LoadingEventProgressReporter>(fs::path(path).filename().replace_extension().string()));
    if (!zone)
        return std::unexpected(std::move(zone.error()));

    auto searchPathsForZone = AutoSearchPaths::GetForGame((*zone)->m_game_id)->GetSearchPathsForZonePath(path);
    {
        std::lock_guard lock(m_search_path_lock);
        for (const auto& searchPathStr : searchPathsForZone)
            m_shared_search_paths.RefSearchPath(searchPathStr);
    }

    auto loadedZone = std::make_unique<LoadedZoneInformation>(std::move(*zone), path, std::move(searchPathsForZone));

    LoadedZoneInformation* loadedZonePtr;
    {
        std::lock_guard lock(m_zone_lock);
        loadedZonePtr = m_loaded_zones.emplace_back(std::move(loadedZone)).get();
    }

    {
        std::shared_lock lock(m_search_path_lock);
        auto* objLoader = IObjLoader::GetObjLoaderForGame(loadedZonePtr->GetZone().m_game_id);
        objLoader->LoadReferencedContainersForZone(m_shared_search_paths.GetSearchPath(), loadedZonePtr->GetZone());
    }

    ui::NotifyZoneLoaded(*loadedZonePtr);

    return loadedZonePtr;
}

std::expected<void, std::string> FastFileContext::UnloadZone(const std::string& zoneName)
{
    std::unique_ptr<LoadedZoneInformation> removedLoadedZone;

    {
        std::lock_guard lock(m_zone_lock);
        const auto existingZone = std::ranges::find_if(m_loaded_zones,
                                                       [&zoneName](const std::unique_ptr<LoadedZoneInformation>& loadedZone)
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
            m_shared_search_paths.UnrefSearchPath(searchPathStr);
    }

    return {};
}

ReadAccess<const std::vector<std::unique_ptr<LoadedZoneInformation>>> FastFileContext::GetLoadedZones()
{
    return ReadAccess<const std::vector<std::unique_ptr<LoadedZoneInformation>>>(std::shared_lock(m_zone_lock), m_loaded_zones);
}

ReadAccess<ISearchPath> FastFileContext::GetSearchPaths()
{
    return ReadAccess(std::shared_lock(m_search_path_lock), m_shared_search_paths.GetSearchPath());
}
