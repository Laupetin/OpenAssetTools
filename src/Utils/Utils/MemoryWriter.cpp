#include "MemoryWriter.h"

#include <cassert>

MemoryWriter::MemoryWriter(uint32_t capacity)
{
    m_dataPointer = new int8_t[capacity];
    m_dataLength = capacity;
    m_currentPosition = 0;
}

MemoryWriter::~MemoryWriter()
{
    Close();
}

uint64_t MemoryWriter::GetLength() const
{
    return m_dataLength;
}

uint64_t MemoryWriter::GetPosition() const
{
    return m_currentPosition;
}

int8_t* MemoryWriter::Data() const
{
    return m_dataPointer;
}

void MemoryWriter::Close()
{
    if (m_dataPointer != nullptr)
    {
        delete[] m_dataPointer;
        m_dataPointer = nullptr;
    }
}

void MemoryWriter::Write(const uint8_t* buffer, uint32_t size)
{
    ValidateCapacity(m_currentPosition + size);

    std::memcpy(m_dataPointer + m_currentPosition, buffer, size);
    m_currentPosition += size;
}

void MemoryWriter::WriteNullTerminatedString(const std::string& string)
{
    Write(reinterpret_cast<const uint8_t*>(string.c_str()), string.size() + 1);
}

void MemoryWriter::Reallocate(uint64_t capacity)
{
    auto tempPtr = static_cast<int8_t*>(std::realloc(m_dataPointer, capacity));

    if (tempPtr != nullptr)
    {
        m_dataPointer = tempPtr;
        m_dataLength = capacity;
    }
}

void MemoryWriter::ValidateCapacity(uint64_t newSize)
{
    if (newSize > m_dataLength)
    {
        auto newCapacity = m_dataLength * 2;
        assert(newCapacity <= UINT64_MAX);
        Reallocate(newCapacity);
    }
}
