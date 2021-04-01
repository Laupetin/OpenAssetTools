#pragma once
#include "Loading/StreamProcessor.h"

class ProcessorIW4xDecryption final : public StreamProcessor
{
    uint8_t m_last_byte;

    static uint8_t RotateLeft(uint8_t value, unsigned count);
    static uint8_t RotateRight(uint8_t value, unsigned count);

public:
    ProcessorIW4xDecryption();

    size_t Load(void* buffer, size_t length) override;
    int64_t Pos() override;
};
