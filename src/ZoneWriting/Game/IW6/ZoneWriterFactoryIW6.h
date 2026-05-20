#pragma once

#include "Writing/IZoneWriterFactory.h"

#include <memory>

namespace IW6
{
    class ZoneWriterFactory final : public IZoneWriterFactory
    {
    public:
        [[nodiscard]] std::unique_ptr<ZoneWriter> CreateWriter(const Zone& zone) const override;
    };
} // namespace IW6
