#include "OutputProcessorDeflate.h"

#include "Utils/ClassUtils.h"
#include "Writing/WritingException.h"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <zlib.h>

class OutputProcessorDeflate::Impl
{
public:
    Impl(OutputProcessorDeflate* baseClass, const size_t bufferSize)
        : m_buffer(std::make_unique<uint8_t[]>(bufferSize)),
          m_buffer_size(bufferSize)
    {
        m_base = baseClass;

        m_stream.zalloc = Z_NULL;
        m_stream.zfree = Z_NULL;
        m_stream.opaque = Z_NULL;
        m_stream.avail_in = 0;
        m_stream.next_in = Z_NULL;
        m_stream.next_out = m_buffer.get();
        m_stream.avail_out = static_cast<unsigned>(m_buffer_size);

        const int ret = deflateInit(&m_stream, Z_DEFAULT_COMPRESSION);

        if (ret != Z_OK)
        {
            throw std::runtime_error("Initializing deflate failed");
        }
    }

    ~Impl()
    {
        deflateEnd(&m_stream);
    }

    Impl(const Impl& other) = delete;
    Impl(Impl&& other) noexcept = default;
    Impl& operator=(const Impl& other) = delete;
    Impl& operator=(Impl&& other) noexcept = default;

    void Write(const void* buffer, const size_t length)
    {
        m_stream.next_in = static_cast<const Bytef*>(buffer);
        m_stream.avail_in = static_cast<unsigned>(length);

        while (m_stream.avail_in > 0)
        {
            if (m_stream.avail_out == 0)
            {
                m_base->m_base_stream->Write(m_buffer.get(), m_buffer_size);
                m_stream.next_out = m_buffer.get();
                m_stream.avail_out = static_cast<unsigned>(m_buffer_size);
            }

            const auto ret = deflate(&m_stream, Z_NO_FLUSH);
            if (ret != Z_OK)
                throw WritingException("Failed to deflate memory of zone.");
        }
    }

    void Flush()
    {
        m_stream.avail_in = 0;
        m_stream.next_in = Z_NULL;
        while (true)
        {
            if (m_stream.avail_out < m_buffer_size)
            {
                m_base->m_base_stream->Write(m_buffer.get(), m_buffer_size - m_stream.avail_out);
                m_stream.next_out = m_buffer.get();
                m_stream.avail_out = static_cast<unsigned>(m_buffer_size);
            }

            const auto ret = deflate(&m_stream, Z_FINISH);
            if (ret == Z_OK)
                continue;

            if (ret != Z_STREAM_END)
                throw WritingException("Failed to flush deflate memory of zone.");

            break;
        }

        if (m_stream.avail_out < m_buffer_size)
        {
            m_base->m_base_stream->Write(m_buffer.get(), m_buffer_size - m_stream.avail_out);
            m_stream.next_out = m_buffer.get();
            m_stream.avail_out = static_cast<unsigned>(m_buffer_size);
        }
    }

    [[nodiscard]] int64_t Pos() const
    {
        return m_base->m_base_stream->Pos();
    }

private:
    z_stream m_stream{};
    OutputProcessorDeflate* m_base;

    std::unique_ptr<uint8_t[]> m_buffer;
    size_t m_buffer_size;
};

OutputProcessorDeflate::OutputProcessorDeflate()
    : m_impl(new Impl(this, DEFAULT_BUFFER_SIZE))
{
}

OutputProcessorDeflate::OutputProcessorDeflate(const size_t bufferSize)
    : m_impl(new Impl(this, bufferSize))
{
}

OutputProcessorDeflate::~OutputProcessorDeflate()
{
    delete m_impl;
    m_impl = nullptr;
}

void OutputProcessorDeflate::Write(const void* buffer, const size_t length)
{
    m_impl->Write(buffer, length);
}

void OutputProcessorDeflate::Flush()
{
    m_impl->Flush();
}

int64_t OutputProcessorDeflate::Pos()
{
    return m_impl->Pos();
}
