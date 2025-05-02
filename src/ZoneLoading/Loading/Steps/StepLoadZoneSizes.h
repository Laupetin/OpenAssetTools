#pragma once

#include "Loading/ILoadingStep.h"

#include <memory>

namespace step
{
    class IStepLoadZoneSizes : public ILoadingStep
    {
    public:
        [[nodiscard]] virtual uint32_t GetSize() const = 0;
        [[nodiscard]] virtual uint32_t GetExternalSize() const = 0;
    };

    std::unique_ptr<IStepLoadZoneSizes> CreateStepLoadZoneSizes();
} // namespace step
