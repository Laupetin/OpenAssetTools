#pragma once
#include <cstdint>
#include <cstddef>

#include "Writing/OutputStreamProcessor.h"

class OutputProcessorDeflate final : public OutputStreamProcessor
{
    class Impl;
    Impl* m_impl;

    static constexpr size_t DEFAULT_BUFFER_SIZE = 0x40000;

public:
    OutputProcessorDeflate();
    OutputProcessorDeflate(size_t bufferSize);
    ~OutputProcessorDeflate() override;
    OutputProcessorDeflate(const OutputProcessorDeflate& other) = delete;
    OutputProcessorDeflate(OutputProcessorDeflate&& other) noexcept = default;
    OutputProcessorDeflate& operator=(const OutputProcessorDeflate& other) = delete;
    OutputProcessorDeflate& operator=(OutputProcessorDeflate&& other) noexcept = default;

    void Write(const void* buffer, size_t length) override;
    void Flush() override;
    int64_t Pos() override;
};