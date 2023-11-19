#pragma once

#include "Loading/IZoneLoaderFactory.h"

#include <string>

namespace T5
{
    class ZoneLoaderFactory final : public IZoneLoaderFactory
    {
        class Impl;

    public:
        ZoneLoader* CreateLoaderForHeader(ZoneHeader& header, std::string& fileName) override;
    };
} // namespace T5
