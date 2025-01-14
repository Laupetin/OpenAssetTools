#pragma once

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

    [[nodiscard]] virtual std::unique_ptr<ZoneWriter> CreateWriter(const Zone& zone) const = 0;

    static const IZoneWriterFactory* GetZoneWriterFactoryForGame(GameId game);
};
