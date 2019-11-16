#pragma once
#include "Dumping/IZoneDumper.h"

class ZoneDumperT6 final : public IZoneDumper
{
public:
    bool CanDumpZone(Zone* zone) override;
    bool DumpZone(Zone* zone, const std::string& basePath) override;
};
