#pragma once

#include "Loading/ILoadingStep.h"

#include <functional>
#include <memory>

namespace step
{
    class StepCallback final : public ILoadingStep
    {
    public:
        explicit StepCallback(std::function<void(ZoneLoader&, ILoadingStream&)> callback);

        void PerformStep(ZoneLoader& loader, ILoadingStream& stream) override;

    private:
        std::function<void(ZoneLoader&, ILoadingStream&)> m_callback;
    };

    std::unique_ptr<ILoadingStep> CreateStepCallback(std::function<void(ZoneLoader&, ILoadingStream&)> callback);
} // namespace step
