#pragma once

#include "Writing/IZoneWriterFactory.h"

#include <memory>

namespace CODO
{
    class ZoneWriterFactory final : public IZoneWriterFactory
    {
    public:
        [[nodiscard]] std::unique_ptr<ZoneWriter> CreateWriter(const Zone& zone) const override;
    };
} // namespace CODO
