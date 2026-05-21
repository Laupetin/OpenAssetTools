#include "StepCallback.h"

namespace step
{
    StepCallback::StepCallback(std::function<void(ZoneLoader&, ILoadingStream&)> callback)
        : m_callback(std::move(callback))
    {
    }

    void StepCallback::PerformStep(ZoneLoader& loader, ILoadingStream& stream) 
    {
        m_callback(loader, stream);
    }

    std::unique_ptr<ILoadingStep> CreateStepCallback(std::function<void(ZoneLoader&, ILoadingStream&)> callback)
    {
        return std::make_unique<StepCallback>(std::move(callback));
    }
} // namespace step
