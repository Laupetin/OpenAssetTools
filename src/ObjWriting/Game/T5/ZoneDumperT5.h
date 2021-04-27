#pragma once
#include "Dumping/IZoneDumper.h"

namespace T5
{
    class ZoneDumper final : public IZoneDumper
    {
    public:
        bool CanHandleZone(AssetDumpingContext& context) const override;
        bool DumpZone(AssetDumpingContext& context) const override;
    };
}
