#pragma once

#include "Loading/IContentLoadingEntryPoint.h"
#include "Loading/ILoadingStep.h"

#include <memory>

namespace step
{
    std::unique_ptr<ILoadingStep>
        CreateStepLoadZoneContent(std::unique_ptr<IContentLoadingEntryPoint> entryPoint, int offsetBlockBitCount, block_t insertBlock);
}
