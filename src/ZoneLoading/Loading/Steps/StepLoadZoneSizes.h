#pragma once

#include "Loading/ILoadingStep.h"

#include <memory>

namespace step
{
    class IStepLoadZoneSizes : public ILoadingStep
    {
    public:
#ifdef ARCH_86
        [[nodiscard]] virtual uint32_t GetSize() const = 0;
        [[nodiscard]] virtual uint32_t GetExternalSize() const = 0;
#else
        [[nodiscard]] virtual uint64_t GetSize() const = 0;
        [[nodiscard]] virtual uint64_t GetExternalSize() const = 0;
#endif
    };

    std::unique_ptr<IStepLoadZoneSizes> CreateStepLoadZoneSizes();
} // namespace step
