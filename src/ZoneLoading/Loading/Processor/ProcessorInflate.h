#pragma once
#include "Loading/StreamProcessor.h"

class ProcessorInflate final : public StreamProcessor
{
    class Impl;
    Impl* m_impl;

    static constexpr size_t DEFAULT_BUFFER_SIZE = 0x2000;

public:
    ProcessorInflate();
    ProcessorInflate(size_t bufferSize);
    ~ProcessorInflate() override;
    ProcessorInflate(const ProcessorInflate& other) = delete;
    ProcessorInflate(ProcessorInflate&& other) noexcept = default;
    ProcessorInflate& operator=(const ProcessorInflate& other) = delete;
    ProcessorInflate& operator=(ProcessorInflate&& other) noexcept = default;

    size_t Load(void* buffer, size_t length) override;
    int64_t Pos() override;
};
