#pragma once

#include <cstdint>
#include <string>

class MemoryWriter
{
public:
    MemoryWriter(uint32_t capacity);
    ~MemoryWriter();

    size_t GetLength() const;
    size_t GetPosition() const;
    int8_t* Data() const;

    void Close();
    void Write(const uint8_t* buffer, uint32_t size);
    void WriteNullTerminatedString(const std::string& string);

    template<class T> void Write(const T data)
    {
        Write((uint8_t*)&data, sizeof(T));
    }

private:
    void Reallocate(size_t capacity);
    void ValidateCapacity(size_t size);

private:
    int8_t* m_dataPointer;
    size_t m_dataLength;
    size_t m_currentPosition;
};
