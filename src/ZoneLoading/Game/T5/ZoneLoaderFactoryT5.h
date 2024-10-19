#pragma once

#include "Loading/IZoneLoaderFactory.h"

#include <string>

namespace T5
{
    class ZoneLoaderFactory final : public IZoneLoaderFactory
    {
    public:
        std::unique_ptr<ZoneLoader> CreateLoaderForHeader(ZoneHeader& header, std::string& fileName) const override;
    };
} // namespace T5
