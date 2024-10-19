#pragma once

#include "Writing/IZoneWriterFactory.h"

#include <memory>

namespace T6
{
    class ZoneWriterFactory final : public IZoneWriterFactory
    {
        class Impl;

    public:
        _NODISCARD std::unique_ptr<ZoneWriter> CreateWriter(Zone* zone) const override;
    };
} // namespace T6
