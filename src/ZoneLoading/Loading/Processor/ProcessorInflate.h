#pragma once

#include "Loading/StreamProcessor.h"

#include <memory>

namespace processor
{
    std::unique_ptr<StreamProcessor> CreateProcessorInflate();
    std::unique_ptr<StreamProcessor> CreateProcessorInflate(size_t bufferSize);
} // namespace processor
