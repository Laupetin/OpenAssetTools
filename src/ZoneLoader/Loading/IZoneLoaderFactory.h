#pragma once

#include "Zone/ZoneTypes.h"
#include "ZoneLoader.h"

class IZoneLoaderFactory
{
public:
    virtual ~IZoneLoaderFactory() = default;

    virtual ZoneLoader* CreateLoaderForHeader(ZoneHeader& header, std::string& fileName) = 0;
};