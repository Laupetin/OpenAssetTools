#pragma once

#include "Loading/ILoadingStep.h"

#include <cstddef>

class StepLoadZoneSizes final : public ILoadingStep
{
public:
    StepLoadZoneSizes();

    void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override;

    [[nodiscard]] size_t GetSize() const;
    [[nodiscard]] size_t GetExternalSize() const;

private:
    size_t m_size;
    size_t m_external_size;
};
