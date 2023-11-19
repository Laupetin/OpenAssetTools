#pragma once

#include "Loading/ILoadingStep.h"

class StepVerifyMagic final : public ILoadingStep
{
    const char* m_magic;
    size_t m_magic_len;

public:
    explicit StepVerifyMagic(const char* magic);

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};
