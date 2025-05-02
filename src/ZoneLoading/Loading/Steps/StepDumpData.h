#pragma once

#include "Loading/ILoadingStep.h"

#include <memory>

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepDumpData(size_t dumpCount);
}
