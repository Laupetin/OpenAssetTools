#pragma once
#include "IXChunkOutputProcessor.h"
#include "Utils/ICapturedDataProvider.h"

class XChunkOutputProcessorSalsa20 final : public IXChunkOutputProcessor, public ICapturedDataProvider
{
    class Impl;
    Impl* m_impl;

public:
    XChunkOutputProcessorSalsa20(int streamCount, std::string& zoneName, const uint8_t* salsa20Key, size_t keySize);
    ~XChunkOutputProcessorSalsa20() override;
    XChunkOutputProcessorSalsa20(const XChunkOutputProcessorSalsa20& other) = delete;
    XChunkOutputProcessorSalsa20(XChunkOutputProcessorSalsa20&& other) noexcept = default;
    XChunkOutputProcessorSalsa20& operator=(const XChunkOutputProcessorSalsa20& other) = delete;
    XChunkOutputProcessorSalsa20& operator=(XChunkOutputProcessorSalsa20&& other) noexcept = default;

    size_t Process(int streamNumber, const uint8_t* input, size_t inputLength, uint8_t* output, size_t outputBufferSize) override;
    void GetCapturedData(const uint8_t** pCapturedData, size_t* pSize) override;
};
