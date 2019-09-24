#include "ProcessorInflate.h"
#include "zlib.h"
#include <exception>
#include "zutil.h"
#include <cstdint>

class ProcessorInflate::ProcessorInflateImpl
{
    z_stream m_stream{};
    uint8_t m_in_buffer[0x800];
    ProcessorInflate* m_base;

public:
    ProcessorInflateImpl(ProcessorInflate* baseClass)
    {
        m_base = baseClass;

        m_stream.zalloc = Z_NULL;
        m_stream.zfree = Z_NULL;
        m_stream.opaque = Z_NULL;
        m_stream.avail_in = 0;
        m_stream.next_in = Z_NULL;

        const int ret = inflateInit2(&m_stream, -DEF_WBITS);

        if(ret != Z_OK)
        {
            throw std::exception("Initializing inflate failed");
        }
    }

    ~ProcessorInflateImpl()
    {
        inflateEnd(&m_stream);
    }

    size_t Load(void* buffer, size_t length)
    {
        m_stream.next_out = static_cast<Bytef*>(buffer);
        m_stream.avail_out = length;

        while(m_stream.avail_out > 0)
        {
            if(m_stream.avail_in == 0)
            {
                m_stream.avail_in = m_base->m_base_stream->Load(m_in_buffer, sizeof(m_in_buffer));

                if(m_stream.avail_in == 0) // EOF
                    return length - m_stream.avail_out;
            }

            inflate(&m_stream, Z_FULL_FLUSH);
        }

        return m_stream.avail_out;
    }
};

ProcessorInflate::ProcessorInflate()
{
    m_impl = new ProcessorInflateImpl(this);
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