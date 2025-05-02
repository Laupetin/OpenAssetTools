#pragma once

#include "Loading/ILoadingStep.h"

#include <memory>

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepAddProcessor(std::unique_ptr<StreamProcessor> streamProcessor);
}
