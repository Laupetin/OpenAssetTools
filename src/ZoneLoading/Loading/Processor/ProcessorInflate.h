#pragma once
#include "Loading/StreamProcessor.h"

class ProcessorInflate final : public StreamProcessor
{
    class ProcessorInflateImpl;
    ProcessorInflateImpl* m_impl;

public:
    ProcessorInflate();
    ~ProcessorInflate() override;

    size_t Load(void* buffer, size_t length) override;
};
