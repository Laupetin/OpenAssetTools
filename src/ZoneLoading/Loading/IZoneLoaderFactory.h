#pragma once

#include "Zone/ZoneTypes.h"
#include "ZoneLoader.h"

#include <memory>

class IZoneLoaderFactory
{
public:
    IZoneLoaderFactory() = default;
    virtual ~IZoneLoaderFactory() = default;
    IZoneLoaderFactory(const IZoneLoaderFactory& other) = default;
    IZoneLoaderFactory(IZoneLoaderFactory&& other) noexcept = default;
    IZoneLoaderFactory& operator=(const IZoneLoaderFactory& other) = default;
    IZoneLoaderFactory& operator=(IZoneLoaderFactory&& other) noexcept = default;

    virtual std::unique_ptr<ZoneLoader> CreateLoaderForHeader(ZoneHeader& header, std::string& fileName) const = 0;

    static const IZoneLoaderFactory* GetZoneLoaderFactoryForGame(GameId game);
};
