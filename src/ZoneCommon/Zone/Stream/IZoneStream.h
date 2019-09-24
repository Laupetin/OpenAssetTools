#pragma once
#include "Zone/ZoneTypes.h"

class IZoneStream
{
public:
    virtual ~IZoneStream() = default;

    virtual void PushBlock(block_t block) = 0;
    virtual block_t PopBlock() = 0;
};