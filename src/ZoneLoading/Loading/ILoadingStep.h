#pragma once

#include "Loading/ZoneLoader.h"
#include "ILoadingStream.h"

class ZoneLoader;

class ILoadingStep
{
public:
    virtual ~ILoadingStep() = default;

    virtual void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) = 0;
};
