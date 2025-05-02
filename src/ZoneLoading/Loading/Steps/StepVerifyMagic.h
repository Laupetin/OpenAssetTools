#pragma once

#include "Loading/ILoadingStep.h"

#include <memory>

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepVerifyMagic(const char* magic);
}
