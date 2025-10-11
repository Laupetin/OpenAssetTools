#include "FastFileContext.h"

#include "ZoneLoading.h"

std::optional<Zone*> FastFileContext::LoadFastFile(const std::string& path)
{
    auto zone = ZoneLoading::LoadZone(path);
    if (!zone)
        return std::nullopt;

    return m_loaded_zones.emplace_back(std::move(zone)).get();
}
