#pragma once

#include "Utils/Result.h"
#include "Zone/Zone.h"

#include <memory>
#include <vector>

class FastFileContext
{
public:
    result::Expected<Zone*, std::string> LoadFastFile(const std::string& path);

    std::vector<std::unique_ptr<Zone>> m_loaded_zones;
};
