#pragma once

#include "Writing/IZoneWriterFactory.h"

#include <memory>

namespace T5
{
    class ZoneWriterFactory final : public IZoneWriterFactory
    {
        class Impl;

    public:
        _NODISCARD std::unique_ptr<ZoneWriter> CreateWriter(Zone* zone) const override;
    };
} // namespace T5
