#pragma once

#include "Loading/ILoadingStep.h"

#include <memory>
#include <string>

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepDumpData(std::string fileName, size_t dumpCount);
}
