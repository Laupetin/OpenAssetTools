#pragma once

#include "Utils/Result.h"
#include "Zone/Zone.h"

#include <memory>
#include <vector>

class FastFileContext
{
public:
    void Destroy();

    result::Expected<Zone*, std::string> LoadFastFile(const std::string& path);
    result::Expected<NoResult, std::string> UnloadZone(const std::string& zoneName);

    std::vector<std::unique_ptr<Zone>> m_loaded_zones;
};
