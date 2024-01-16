#include "AbstractCacheReader.h"

using namespace cache;

AbstractCacheReader::AbstractCacheReader(std::istream& stream, MemoryManager& memory)
    : m_memory(memory),
      m_stream(stream)
{
}

void AbstractCacheReader::Read(void* data, const size_t dataSize) const
{
    m_stream.read(static_cast<char*>(data), dataSize);

    const auto readCount = m_stream.gcount();
    if (readCount != dataSize)
    {
        const auto pos = m_stream.tellg();
        throw std::exception("Bad read");
    }
}

bool AbstractCacheReader::ReadBool() const
{
    uint8_t value;
    Read(&value, sizeof(value));

    return value != 0;
}

const char* AbstractCacheReader::ReadString() const
{
    uint16_t len = 0;
    Read(&len, sizeof(len));

    auto* str = static_cast<char*>(m_memory.Alloc(static_cast<size_t>(len) + 1));
    Read(str, len);
    str[len] = '\0';

    return str;
}
