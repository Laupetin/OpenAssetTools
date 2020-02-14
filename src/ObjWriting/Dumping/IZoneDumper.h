#pragma once

#include "Zone/Zone.h"
#include "Utils/FileAPI.h"

class IZoneDumper
{
public:
    virtual ~IZoneDumper() = default;

    virtual bool CanHandleZone(Zone* zone) const = 0;
    virtual bool DumpZone(Zone* zone, const std::string& basePath) const = 0;
    virtual bool WriteZoneDefinition(Zone* zone, FileAPI::File* file) const = 0;
};