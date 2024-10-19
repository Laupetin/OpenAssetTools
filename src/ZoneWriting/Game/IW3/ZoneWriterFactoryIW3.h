#pragma once

#include "Writing/IZoneWriterFactory.h"

#include <memory>

namespace IW3
{
    class ZoneWriterFactory final : public IZoneWriterFactory
    {
    public:
        _NODISCARD std::unique_ptr<ZoneWriter> CreateWriter(Zone* zone) const override;
    };
} // namespace IW3
