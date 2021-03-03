#pragma once

#include "ContentLister/ZoneDefWriter.h"

namespace IW4
{
    class ZoneDefWriter final : public IZoneDefWriter
    {
    public:
        bool CanHandleZone(Zone* zone) const override;
        void WriteZoneDef(Zone* zone, std::ostream& stream) const override;
    };
}