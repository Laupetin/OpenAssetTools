#include "ProcessorIW4xDecryption.h"

#include <cassert>

ProcessorIW4xDecryption::ProcessorIW4xDecryption()
    : m_last_byte(0u)
{
}

uint8_t ProcessorIW4xDecryption::RotateLeft(const uint8_t value, const unsigned count)
{
    assert(count < sizeof(value) * 8);
    return static_cast<uint8_t>(value << count | (value >> ((sizeof(value) * 8) - count)));
}

uint8_t ProcessorIW4xDecryption::RotateRight(uint8_t value, const unsigned count)
{
    assert(count < sizeof(value) * 8);
    return static_cast<uint8_t>(value >> count | (value << ((sizeof(value) * 8) - count)));
}

size_t ProcessorIW4xDecryption::Load(void* buffer, const size_t length)
{
    const auto readLen = m_base_stream->Load(buffer, length);

    auto* charBuffer = static_cast<uint8_t*>(buffer);
    for (auto i = 0u; i < readLen; i++)
    {
        auto value = charBuffer[i];
        value ^= m_last_byte;
        value = RotateLeft(value, 4);
        value ^= -1;
        value = RotateRight(value, 6);

        charBuffer[i] = value;
        m_last_byte = value;
    }

    return readLen;
}

int64_t ProcessorIW4xDecryption::Pos()
{
    return m_base_stream->Pos();
}
