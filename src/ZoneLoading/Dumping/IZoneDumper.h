#pragma once

#include "Zone/Zone.h"

class IZoneDumper
{
public:
    virtual ~IZoneDumper() = default;

    virtual bool CanDumpZone(Zone* zone) = 0;
    virtual bool DumpZone(Zone* zone, const std::string& basePath) = 0;
};