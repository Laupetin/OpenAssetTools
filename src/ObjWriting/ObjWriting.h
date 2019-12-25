#pragma once

#include "Zone/Zone.h"
#include "Utils/FileAPI.h"
#include <string>

class ObjWriting
{
public:
    static bool DumpZone(Zone* zone, const std::string& basePath);
    static bool WriteZoneDefinition(Zone* zone, FileAPI::File* file, bool minimalistic);
};
