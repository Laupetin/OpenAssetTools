#pragma once

#include "Loading/ILoadingStep.h"

class StepVerifyMagic final : public ILoadingStep
{
    const char* m_magic;

public:
    explicit StepVerifyMagic(const char* magic);

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};