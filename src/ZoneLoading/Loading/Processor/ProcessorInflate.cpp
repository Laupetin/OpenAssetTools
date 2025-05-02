#include "ProcessorInflate.h"

#include "Loading/Exception/InvalidCompressionException.h"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <zlib.h>
#include <zutil.h>

namespace
{
    constexpr size_t DEFAULT_BUFFER_SIZE = 0x2000;

    class ProcessorInflate final : public StreamProcessor
    {
    public:
        explicit ProcessorInflate(const size_t bufferSize)
            : m_buffer(std::make_unique<uint8_t[]>(bufferSize)),
              m_buffer_size(bufferSize)
        {
            m_stream.zalloc = Z_NULL;
            m_stream.zfree = Z_NULL;
            m_stream.opaque = Z_NULL;
            m_stream.avail_in = 0;
            m_stream.next_in = Z_NULL;

            const int ret = inflateInit(&m_stream);

            if (ret != Z_OK)
                throw std::runtime_error("Initializing inflate failed");
        }

        ~ProcessorInflate() override
        {
            inflateEnd(&m_stream);
        }

        ProcessorInflate(const ProcessorInflate& other) = delete;
        ProcessorInflate(ProcessorInflate&& other) noexcept = default;
        ProcessorInflate& operator=(const ProcessorInflate& other) = delete;
        ProcessorInflate& operator=(ProcessorInflate&& other) noexcept = default;

        size_t Load(void* buffer, const size_t length) override
        {
            m_stream.next_out = static_cast<Bytef*>(buffer);
            m_stream.avail_out = static_cast<unsigned>(length);

            while (m_stream.avail_out > 0)
            {
                if (m_stream.avail_in == 0)
                {
                    m_stream.avail_in = static_cast<unsigned>(m_base_stream->Load(m_buffer.get(), m_buffer_size));
                    m_stream.next_in = m_buffer.get();

                    if (m_stream.avail_in == 0) // EOF
                        return length - m_stream.avail_out;
                }

                const auto ret = inflate(&m_stream, Z_SYNC_FLUSH);

                if (ret < 0)
                    throw InvalidCompressionException();
            }

            return length - m_stream.avail_out;
        }

        int64_t Pos() override
        {
            return m_base_stream->Pos();
        }

    private:
        z_stream m_stream{};
        std::unique_ptr<uint8_t[]> m_buffer;
        size_t m_buffer_size;
    };
} // namespace

namespace processor
{
    std::unique_ptr<StreamProcessor> CreateProcessorInflate()
    {
        return std::make_unique<ProcessorInflate>(DEFAULT_BUFFER_SIZE);
    }

    std::unique_ptr<StreamProcessor> CreateProcessorInflate(size_t bufferSize)
    {
        return std::make_unique<ProcessorInflate>(bufferSize);
    }
} // namespace processor
