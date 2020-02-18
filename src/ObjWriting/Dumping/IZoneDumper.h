#pragma once

#include "Zone/Zone.h"

class IZoneDumper
{
public:
    virtual ~IZoneDumper() = default;

    virtual bool CanHandleZone(Zone* zone) const = 0;
    virtual bool DumpZone(Zone* zone, const std::string& basePath) const = 0;
};