#pragma once
#include <string>

#include "Zone/Zone.h"

class ZoneLoading
{
public:
    static std::unique_ptr<Zone> LoadZone(const std::string& path);
};
