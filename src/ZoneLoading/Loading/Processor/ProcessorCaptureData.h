#pragma once

#include "Loading/StreamProcessor.h"
#include "Utils/ICapturedDataProvider.h"

#include <memory>

namespace processor
{
    class IProcessorCaptureData : public StreamProcessor, public ICapturedDataProvider
    {
    };

    std::unique_ptr<IProcessorCaptureData> CreateProcessorCaptureData(size_t captureSize);
} // namespace processor
