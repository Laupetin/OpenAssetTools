#pragma once

#include "Loading/ILoadingStep.h"

#include <memory>
#include <string>

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepVerifyFileName(std::string fileName, size_t fileNameBufferSize);
}
