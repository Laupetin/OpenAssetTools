#pragma once

#include "Loading/ILoadingStep.h"

class StepVerifyFileName final : public ILoadingStep
{
    std::string m_file_name;
    size_t m_file_name_buffer_size;

public:
    explicit StepVerifyFileName(std::string fileName, size_t fileNameBufferSize);

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};