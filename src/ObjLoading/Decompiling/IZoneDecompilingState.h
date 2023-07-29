#pragma once
#include "Zone/Zone.h"

class IZoneDecompilingState
{
protected:
    IZoneDecompilingState() = default;

public:
    virtual ~IZoneDecompilingState() = default;
    IZoneDecompilingState(const IZoneDecompilingState& other) = default;
    IZoneDecompilingState(IZoneDecompilingState&& other) noexcept = default;
    IZoneDecompilingState& operator=(const IZoneDecompilingState& other) = default;
    IZoneDecompilingState& operator=(IZoneDecompilingState&& other) noexcept = default;

    virtual void SetZone(Zone* zone)
    {
        // Do nothing by default
    }
};
