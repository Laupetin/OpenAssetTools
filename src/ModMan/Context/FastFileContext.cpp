#include "FastFileContext.h"

#include "ZoneLoading.h"

result::Expected<Zone*, std::string> FastFileContext::LoadFastFile(const std::string& path)
{
    auto zone = ZoneLoading::LoadZone(path);
    if (!zone)
        return result::Unexpected(std::move(zone.error()));

    return m_loaded_zones.emplace_back(std::move(*zone)).get();
}
