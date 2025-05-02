#pragma once

#include "ILoadingStream.h"
#include "Loading/ZoneLoader.h"

class ZoneLoader;

class ILoadingStep
{
public:
    ILoadingStep() = default;
    virtual ~ILoadingStep() = default;
    ILoadingStep(const ILoadingStep& other) = default;
    ILoadingStep(ILoadingStep&& other) noexcept = default;
    ILoadingStep& operator=(const ILoadingStep& other) = default;
    ILoadingStep& operator=(ILoadingStep&& other) noexcept = default;

    virtual void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) = 0;
};
