#pragma once
#include "Zone/Zone.h"

#include <memory>
#include <optional>
#include <vector>

class FastFileContext
{
public:
    std::optional<Zone*> LoadFastFile(const std::string& path);

    std::vector<std::unique_ptr<Zone>> m_loaded_zones;
};
