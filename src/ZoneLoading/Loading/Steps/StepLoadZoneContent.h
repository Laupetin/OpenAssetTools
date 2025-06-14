#pragma once

#include "Loading/IContentLoadingEntryPoint.h"
#include "Loading/ILoadingStep.h"
#include "Zone/Stream/ZoneInputStream.h"

#include <functional>
#include <memory>

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepLoadZoneContent(std::function<std::unique_ptr<IContentLoadingEntryPoint>(ZoneInputStream&)> entryPointFactory,
                                                            unsigned pointerBitCount,
                                                            unsigned offsetBlockBitCount,
                                                            block_t insertBlock,
                                                            MemoryManager& memory);
}
