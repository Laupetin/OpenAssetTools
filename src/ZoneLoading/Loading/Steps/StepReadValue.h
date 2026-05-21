#pragma once

#include "Loading/ILoadingStep.h"

#include <memory>

namespace step
{
    template<typename T> class StepReadValue final : public ILoadingStep
    {
    public:
        explicit StepReadValue(T* value)
            : m_value(value)
        {
        }

        void PerformStep(ZoneLoader&, ILoadingStream& stream) override
        {
            stream.Load(m_value, sizeof(T));
        }

    private:
        T* m_value;
    };

    template<typename T> std::unique_ptr<ILoadingStep> CreateStepReadValue(T* value)
    {
        return std::make_unique<StepReadValue<T>>(value);
    }
} // namespace step
