#pragma once
#include "Zone/Zone.h"

class IZoneAssetDumperState
{
protected:
    IZoneAssetDumperState() = default;

public:
    virtual ~IZoneAssetDumperState() = default;
    IZoneAssetDumperState(const IZoneAssetDumperState& other) = default;
    IZoneAssetDumperState(IZoneAssetDumperState&& other) noexcept = default;
    IZoneAssetDumperState& operator=(const IZoneAssetDumperState& other) = default;
    IZoneAssetDumperState& operator=(IZoneAssetDumperState&& other) noexcept = default;

    virtual void SetZone(Zone* zone)
    {
        // Do nothing by default
    }
};
