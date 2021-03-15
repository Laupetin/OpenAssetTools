#pragma once

#include <cstddef>

#include "Utils/ClassUtils.h"
#include "Loading/ILoadingStep.h"

class StepLoadZoneSizes final : public ILoadingStep
{
    size_t m_size;
    size_t m_external_size;

public:
    StepLoadZoneSizes();

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;

    _NODISCARD size_t GetSize() const;
    _NODISCARD size_t GetExternalSize() const;
};
