#pragma once

#include "Loading/ILoadingStep.h"
#include "Utils/ClassUtils.h"

#include <cstddef>

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
