#include "ProcessorInflate.h"

#include <exception>
#include <cstdint>
#include <memory>
#include <zlib.h>
#include <zutil.h>

#include "Loading/Exception/InvalidCompressionException.h"

class ProcessorInflate::Impl
{
    z_stream m_stream{};
    ProcessorInflate* m_base;

    std::unique_ptr<uint8_t[]> m_buffer;
    size_t m_buffer_size;

public:
    Impl(ProcessorInflate* baseClass, const size_t bufferSize)
        : m_buffer(std::make_unique<uint8_t[]>(bufferSize)),
          m_buffer_size(bufferSize)
    {
        m_base = baseClass;

        m_stream.zalloc = Z_NULL;
        m_stream.zfree = Z_NULL;
        m_stream.opaque = Z_NULL;
        m_stream.avail_in = 0;
        m_stream.next_in = Z_NULL;

        const int ret = inflateInit(&m_stream);

        if (ret != Z_OK)
        {
            throw std::exception("Initializing inflate failed");
        }
    }

    ~Impl()
    {
        inflateEnd(&m_stream);
    }

    Impl(const Impl& other) = delete;
    Impl(Impl&& other) noexcept = default;
    Impl& operator=(const Impl& other) = delete;
    Impl& operator=(Impl&& other) noexcept = default;

    size_t Load(void* buffer, const size_t length)
    {
        m_stream.next_out = static_cast<Bytef*>(buffer);
        m_stream.avail_out = length;

        while (m_stream.avail_out > 0)
        {
            if (m_stream.avail_in == 0)
            {
                m_stream.avail_in = m_base->m_base_stream->Load(m_buffer.get(), m_buffer_size);

                if (m_stream.avail_in == 0) // EOF
                    return length - m_stream.avail_out;
            }

            auto ret = inflate(&m_stream, Z_SYNC_FLUSH);

            if(ret < 0)
                throw InvalidCompressionException();
        }

        return m_stream.avail_out;
    }
};

ProcessorInflate::ProcessorInflate()
    : ProcessorInflate(DEFAULT_BUFFER_SIZE)
{
}

ProcessorInflate::ProcessorInflate(const size_t bufferSize)
    : m_impl(new Impl(this, bufferSize))
{
}

ProcessorInflate::~ProcessorInflate()
{
    delete m_impl;
    m_impl = nullptr;
}

size_t ProcessorInflate::Load(void* buffer, const size_t length)
{
    return m_impl->Load(buffer, length);
}

int64_t ProcessorInflate::Pos()
{
    return m_base_stream->Pos();
}
