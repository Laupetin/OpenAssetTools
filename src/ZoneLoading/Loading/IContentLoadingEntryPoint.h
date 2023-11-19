#pragma once

#include "Zone/Stream/IZoneInputStream.h"
#include "Zone/Zone.h"

class IContentLoadingEntryPoint
{
public:
    virtual ~IContentLoadingEntryPoint() = default;

    virtual void Load(Zone* zone, IZoneInputStream* stream) = 0;
};
