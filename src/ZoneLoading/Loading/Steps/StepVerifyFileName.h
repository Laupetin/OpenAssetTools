#pragma once

#include "Loading/ILoadingStep.h"

class StepVerifyFileName final : public ILoadingStep
{
public:
    explicit StepVerifyFileName(std::string fileName, size_t fileNameBufferSize);

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;

private:
    std::string m_expected_file_name;
    size_t m_file_name_buffer_size;
};
