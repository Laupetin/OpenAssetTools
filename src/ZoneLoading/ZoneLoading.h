#pragma once
#include "Zone/Zone.h"

#include <string>

class ZoneLoading
{
public:
    static std::unique_ptr<Zone> LoadZone(const std::string& path);
};
