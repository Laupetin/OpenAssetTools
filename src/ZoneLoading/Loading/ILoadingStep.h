#pragma once

#include "ILoadingStream.h"
#include "Loading/ZoneLoader.h"

class ZoneLoader;

class ILoadingStep
{
public:
    virtual ~ILoadingStep() = default;

    virtual void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) = 0;
};
