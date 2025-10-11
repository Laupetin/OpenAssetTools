#include "FastFileContext.h"

#include "ZoneLoading.h"

bool FastFileContext::LoadFastFile(const std::string& path)
{
    m_loaded_zones.emplace_back(ZoneLoading::LoadZone(path));
    return true;
}
