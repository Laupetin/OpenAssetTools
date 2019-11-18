#pragma once
#include "Dumping/IZoneDumper.h"

class ZoneDumperT6 final : public IZoneDumper
{
public:
    bool CanHandleZone(Zone* zone) override;
    bool DumpZone(Zone* zone, const std::string& basePath) override;
    bool WriteZoneDefinition(Zone* zone, FileAPI::File* file, bool minimalistic) override;
};
