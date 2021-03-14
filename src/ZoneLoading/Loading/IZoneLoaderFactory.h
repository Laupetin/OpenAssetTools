#pragma once

#include "Zone/ZoneTypes.h"
#include "ZoneLoader.h"

class IZoneLoaderFactory
{
public:
    IZoneLoaderFactory() = default;
    virtual ~IZoneLoaderFactory() = default;
    IZoneLoaderFactory(const IZoneLoaderFactory& other) = default;
    IZoneLoaderFactory(IZoneLoaderFactory&& other) noexcept = default;
    IZoneLoaderFactory& operator=(const IZoneLoaderFactory& other) = default;
    IZoneLoaderFactory& operator=(IZoneLoaderFactory&& other) noexcept = default;

    virtual ZoneLoader* CreateLoaderForHeader(ZoneHeader& header, std::string& fileName) = 0;
};