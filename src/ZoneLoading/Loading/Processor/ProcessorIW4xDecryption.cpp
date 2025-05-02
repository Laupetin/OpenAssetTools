#include "ProcessorIW4xDecryption.h"

#include <cassert>

namespace
{
    uint8_t RotateLeft(const uint8_t value, const unsigned count)
    {
        assert(count < sizeof(value) * 8);
        return static_cast<uint8_t>(value << count | (value >> ((sizeof(value) * 8) - count)));
    }

    uint8_t RotateRight(const uint8_t value, const unsigned count)
    {
        assert(count < sizeof(value) * 8);
        return static_cast<uint8_t>(value >> count | (value << ((sizeof(value) * 8) - count)));
    }

    class ProcessorIW4xDecryption final : public StreamProcessor
    {
    public:
        ProcessorIW4xDecryption()
            : m_last_byte(0u)
        {
        }

        size_t Load(void* buffer, const size_t length) override
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

        int64_t Pos() override
        {
            return m_base_stream->Pos();
        }

    private:
        uint8_t m_last_byte;
    };
} // namespace

namespace processor
{
    std::unique_ptr<StreamProcessor> CreateProcessorIW4xDecryption()
    {
        return std::make_unique<ProcessorIW4xDecryption>();
    }
} // namespace processor
