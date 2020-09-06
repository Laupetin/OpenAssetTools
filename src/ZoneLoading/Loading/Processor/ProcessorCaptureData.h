#pragma once
#include <memory>

#include "Loading/StreamProcessor.h"
#include "Loading/ICapturedDataProvider.h"

class ProcessorCaptureData final : public StreamProcessor, public ICapturedDataProvider
{
    std::unique_ptr<uint8_t[]> m_data;
    const size_t m_capture_size;
    size_t m_captured_data_size;

public:
    explicit ProcessorCaptureData(size_t captureSize);
    ~ProcessorCaptureData() override;

    size_t Load(void* buffer, size_t length) override;
    int64_t Pos() override;
    void GetCapturedData(const uint8_t** pCapturedData, size_t* pSize) override;
};
