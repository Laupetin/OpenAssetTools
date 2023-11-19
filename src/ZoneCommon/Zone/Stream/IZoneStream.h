#pragma once
#include "Zone/ZoneTypes.h"

class IZoneStream
{
public:
    IZoneStream() = default;
    virtual ~IZoneStream() = default;
    IZoneStream(const IZoneStream& other) = default;
    IZoneStream(IZoneStream&& other) noexcept = default;
    IZoneStream& operator=(const IZoneStream& other) = default;
    IZoneStream& operator=(IZoneStream&& other) noexcept = default;

    virtual void PushBlock(block_t block) = 0;
    virtual block_t PopBlock() = 0;
};
