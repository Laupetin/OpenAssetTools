#pragma once

#include "AssetDumpingContext.h"

class IZoneDumper
{
public:
    IZoneDumper() = default;
    virtual ~IZoneDumper() = default;
    IZoneDumper(const IZoneDumper& other) = default;
    IZoneDumper(IZoneDumper&& other) noexcept = default;
    IZoneDumper& operator=(const IZoneDumper& other) = default;
    IZoneDumper& operator=(IZoneDumper&& other) noexcept = default;

    virtual bool CanHandleZone(AssetDumpingContext& assetDumpingContext) const = 0;
    virtual bool DumpZone(AssetDumpingContext& assetDumpingContext) const = 0;
};
