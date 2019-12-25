#pragma once
#include "Zone/Zone.h"
#include <string>

class ZoneLoading
{
public:
    static Zone* LoadZone(const std::string& path);
};
