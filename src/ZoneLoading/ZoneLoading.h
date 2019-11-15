#pragma once
#include "Zone/Zone.h"
#include <string>
#include "Utils/FileAPI.h"

class ZoneLoading
{
public:
    static Zone* LoadZone(const std::string& path);
    static bool DumpZone(Zone* zone, const std::string& basePath);
    static bool WriteZoneDefinition(Zone* zone, FileAPI::File* file, bool minimalistic);
};
