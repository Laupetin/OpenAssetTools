#pragma once

#include <vector>
#include <memory>

class InMemoryZoneData
{
    static constexpr size_t BUFFER_SIZE = 0x10000;

public:
    class MemoryBuffer
    {
    public:
        std::unique_ptr<char[]> m_data;
        size_t m_size;

        explicit MemoryBuffer(size_t size);
    };

    int64_t m_total_size;
    std::vector<MemoryBuffer> m_buffers;

    InMemoryZoneData();
    void* GetBufferOfSize(size_t size);
};