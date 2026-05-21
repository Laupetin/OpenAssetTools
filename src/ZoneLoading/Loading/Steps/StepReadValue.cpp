#include "StepReadValue.h"


namespace step
{
    template<typename T> std::unique_ptr<ILoadingStep> CreateStepReadValue(T* value);
} // namespace step
