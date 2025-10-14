#include "FastFileContext.h"

#include "Web/Binds/ZoneBinds.h"
#include "Web/UiCommunication.h"
#include "ZoneLoading.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace
{
    constexpr double MIN_PROGRESS_TO_REPORT = 0.005;

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
            const double percentage = static_cast<double>(current) / static_cast<double>(total);

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

result::Expected<Zone*, std::string> FastFileContext::LoadFastFile(const std::string& path)
{
    auto zone = ZoneLoading::LoadZone(path, std::make_unique<LoadingEventProgressReporter>(fs::path(path).filename().replace_extension().string()));
    if (!zone)
        return result::Unexpected(std::move(zone.error()));

    auto* result = m_loaded_zones.emplace_back(std::move(*zone)).get();

    ui::NotifyZoneLoaded(result->m_name, path);

    return result;
}

result::Expected<NoResult, std::string> FastFileContext::UnloadZone(const std::string& zoneName)
{
    const auto existingZone = std::ranges::find_if(m_loaded_zones,
                                                   [&zoneName](const std::unique_ptr<Zone>& zone)
                                                   {
                                                       return zone->m_name == zoneName;
                                                   });

    if (existingZone != m_loaded_zones.end())
    {
        m_loaded_zones.erase(existingZone);
        ui::NotifyZoneUnloaded(zoneName);
        return NoResult();
    }

    return result::Unexpected(std::format("No zone with name {} loaded", zoneName));
}
