#pragma once

#include "Utils/ClassUtils.h"
#include "Zone/Zone.h"
#include "ZoneWriter.h"

class IZoneWriterFactory
{
public:
    IZoneWriterFactory() = default;
    virtual ~IZoneWriterFactory() = default;
    IZoneWriterFactory(const IZoneWriterFactory& other) = default;
    IZoneWriterFactory(IZoneWriterFactory&& other) noexcept = default;
    IZoneWriterFactory& operator=(const IZoneWriterFactory& other) = default;
    IZoneWriterFactory& operator=(IZoneWriterFactory&& other) noexcept = default;

    _NODISCARD virtual bool SupportsZone(Zone* zone) const = 0;
    _NODISCARD virtual std::unique_ptr<ZoneWriter> CreateWriter(Zone* zone) const = 0;
};
