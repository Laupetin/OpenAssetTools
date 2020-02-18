#pragma once
#include "Dumping/IZoneDumper.h"

class ZoneDumperT6 final : public IZoneDumper
{
public:
    bool CanHandleZone(Zone* zone) const override;
    bool DumpZone(Zone* zone, const std::string& basePath) const override;
};
