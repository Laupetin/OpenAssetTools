#pragma once
#include "Zone/Zone.h"

#include <memory>
#include <vector>

class FastFileContext
{
public:
    bool LoadFastFile(const std::string& path);

    std::vector<std::unique_ptr<Zone>> m_loaded_zones;
};
