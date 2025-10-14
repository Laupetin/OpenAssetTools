#include "FastFileContext.h"

#include "Web/Binds/ZoneBinds.h"
#include "Web/UiCommunication.h"
#include "ZoneLoading.h"

void FastFileContext::Destroy()
{
    // Unload all zones
    m_loaded_zones.clear();
}

result::Expected<Zone*, std::string> FastFileContext::LoadFastFile(const std::string& path)
{
    auto zone = ZoneLoading::LoadZone(path, std::nullopt);
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
