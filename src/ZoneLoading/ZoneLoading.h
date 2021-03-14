#pragma once
#include <string>

#include "Zone/Zone.h"

class ZoneLoading
{
public:
    static Zone* LoadZone(const std::string& path);
};
