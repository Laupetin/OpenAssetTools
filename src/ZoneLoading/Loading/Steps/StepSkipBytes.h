#pragma once

#include "Loading/ILoadingStep.h"

#include <memory>

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepSkipBytes(size_t skipCount);
}
