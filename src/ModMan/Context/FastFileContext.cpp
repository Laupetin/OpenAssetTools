#include "FastFileContext.h"

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

LoadedZone::LoadedZone(std::unique_ptr<Zone> zone, std::string filePath)
    : m_zone(std::move(zone)),
      m_file_path(std::move(filePath))
{
}

void FastFileContext::Destroy()
{
    // Unload all zones
    m_loaded_zones.clear();
}

result::Expected<LoadedZone*, std::string> FastFileContext::LoadFastFile(const std::string& path)
{
    auto zone = ZoneLoading::LoadZone(path, std::make_unique<LoadingEventProgressReporter>(fs::path(path).filename().replace_extension().string()));
    if (!zone)
        return result::Unexpected(std::move(zone.error()));

    auto loadedZone = std::make_unique<LoadedZone>(std::move(*zone), path);

    LoadedZone* result;
    {
        std::lock_guard lock(m_zone_lock);
        result = m_loaded_zones.emplace_back(std::move(loadedZone)).get();
    }

    ui::NotifyZoneLoaded(*result);

    return result;
}

result::Expected<NoResult, std::string> FastFileContext::UnloadZone(const std::string& zoneName)
{
    {
        std::lock_guard lock(m_zone_lock);
        const auto existingZone = std::ranges::find_if(m_loaded_zones,
                                                       [&zoneName](const std::unique_ptr<LoadedZone>& loadedZone)
                                                       {
                                                           return loadedZone->m_zone->m_name == zoneName;
                                                       });

        if (existingZone != m_loaded_zones.end())
        {
            m_loaded_zones.erase(existingZone);
            ui::NotifyZoneUnloaded(zoneName);
            return NoResult();
        }
    }

    return result::Unexpected(std::format("No zone with name {} loaded", zoneName));
}
