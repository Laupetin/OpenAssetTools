#pragma once

#include "Zone/Zone.h"
#include "Zone/Stream/IZoneInputStream.h"

class IContentLoadingEntryPoint
{
public:
    virtual ~IContentLoadingEntryPoint() = default;

    virtual void Load(Zone* zone, IZoneInputStream* stream) = 0;
};