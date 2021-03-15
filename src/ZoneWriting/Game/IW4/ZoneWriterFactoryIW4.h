#pragma once
#include "Writing/IZoneWriterFactory.h"

namespace IW4
{
    class ZoneWriterFactory final : public IZoneWriterFactory
    {
    public:
        _NODISCARD bool SupportsZone(Zone* zone) const override;
        _NODISCARD std::unique_ptr<ZoneWriter> CreateWriter(Zone* zone) const override;
    };
}
