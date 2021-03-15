#include "InMemoryZoneData.h"

#include <cassert>

InMemoryZoneData::InMemoryZoneData()
    : m_total_size(0)
{
    m_buffers.emplace_back(BUFFER_SIZE);
}

InMemoryZoneData::MemoryBuffer::MemoryBuffer(const size_t size)
    : m_data(std::make_unique<char[]>(size)),
      m_size(0)
{
}

void* InMemoryZoneData::GetBufferOfSize(const size_t size)
{
    assert(size <= BUFFER_SIZE);

    if(m_buffers.back().m_size + size > BUFFER_SIZE)
    {
        m_buffers.emplace_back(BUFFER_SIZE);
    }

    auto& backBuffer = m_buffers.back();
    void* result = &backBuffer.m_data[backBuffer.m_size];
    backBuffer.m_size += size;
    m_total_size += size;
    return result;
}
