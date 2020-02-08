#include "IPakEntryReadStream.h"
#include <cassert>
#include <stdexcept>

int64_t IPakEntryReadStream::Align(const int64_t num, const int64_t alignTo)
{
    return (num + alignTo - 1) / alignTo * alignTo;
}

IPakEntryReadStream::IPakEntryReadStream(IFile* file, IPakStreamManager* streamManager, std::mutex* readMutex, const int64_t startOffset, const size_t length)
{
    m_file = file;
    m_base_pos = startOffset;
    m_end_pos = startOffset + length;
    m_buffer = new uint8_t[IPAK_CHUNK_SIZE * IPAK_CHUNK_COUNT_PER_READ];
    m_stream_manager = streamManager;
    m_read_mutex = readMutex;

    m_buffer_pos = 0;
    m_pos = m_base_pos;
}

IPakEntryReadStream::~IPakEntryReadStream()
{
    Close();
}

bool IPakEntryReadStream::IsOpen()
{
    return m_file != nullptr;
}

size_t IPakEntryReadStream::Read(void* buffer, const size_t elementSize, const size_t elementCount)
{
    const size_t readSize = elementCount * elementSize;
    size_t chunksToRead = Align((m_pos % IPAK_CHUNK_SIZE) + readSize, IPAK_CHUNK_SIZE) / IPAK_CHUNK_SIZE;

    return 0;
}

size_t IPakEntryReadStream::Write(const void* data, size_t elementSize, size_t elementCount)
{
    // This is not meant for writing.
    assert(false);
    throw std::runtime_error("This is not a stream for output!");
}

void IPakEntryReadStream::Skip(const int64_t amount)
{
    if(amount > 0)
    {
        m_pos += amount;

        if (m_pos > m_end_pos)
            m_pos = m_end_pos;
        else if (m_pos < m_base_pos)
            m_pos = m_base_pos;
    }
}

size_t IPakEntryReadStream::Printf(const char* fmt, ...)
{
    // This is not meant for writing.
    assert(false);
    throw std::runtime_error("This is not a stream for output!");
}

int64_t IPakEntryReadStream::Pos()
{
    return m_pos - m_base_pos;
}

void IPakEntryReadStream::Goto(const int64_t pos)
{
    if(pos <= 0)
    {
        m_pos = m_base_pos;
    }
    else if(pos < m_end_pos - m_base_pos)
    {
        m_pos = pos + m_base_pos;
    }
    else
    {
        m_pos = m_end_pos;
    }
}

void IPakEntryReadStream::GotoEnd()
{
    m_pos = m_end_pos;
}

void IPakEntryReadStream::Close()
{
    if(IsOpen())
    {
        m_file = nullptr;

        delete[] m_buffer;
        m_buffer = nullptr;

        m_stream_manager->OnCloseStream(this);
    }
}
