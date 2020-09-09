#pragma once
#include "Dumping/IZoneDumper.h"

namespace IW4
{
    class ZoneDumper final : public IZoneDumper
    {
    public:
        bool CanHandleZone(Zone* zone) const override;
        bool DumpZone(Zone* zone, const std::string& basePath) const override;
    };
}
