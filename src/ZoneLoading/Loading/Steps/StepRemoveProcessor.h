#pragma once

#include "Loading/ILoadingStep.h"
#include "Loading/StreamProcessor.h"

#include <memory>

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepRemoveProcessor(const StreamProcessor* streamProcessor);
}
