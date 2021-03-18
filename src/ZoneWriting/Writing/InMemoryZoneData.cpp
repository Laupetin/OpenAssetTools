#include "InMemoryZoneData.h"

#include <stdexcept>

InMemoryZoneData::InMemoryZoneData()
    : m_total_size(0)
{
    m_buffers.emplace_back(BUFFER_SIZE);
}

InMemoryZoneData::MemoryBuffer::MemoryBuffer(const size_t size)
    : m_data(std::make_unique<char[]>(size)),
      m_size(0)
{
    if (!m_data)
        throw std::runtime_error("Failed to allocate memory for memory buffer.");
}

void* InMemoryZoneData::GetBufferOfSize(const size_t size)
{
    if(size > BUFFER_SIZE)
    {
        m_buffers.emplace_back(size);
    }
    else
    {
        if (m_buffers.back().m_size + size > BUFFER_SIZE)
        {
            m_buffers.emplace_back(BUFFER_SIZE);
        }
    }

    auto& backBuffer = m_buffers.back();
    void* result = &backBuffer.m_data[backBuffer.m_size];
    backBuffer.m_size += size;
    m_total_size += size;
    return result;
}
