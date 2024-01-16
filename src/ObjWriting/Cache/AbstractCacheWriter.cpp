#include "AbstractCacheWriter.h"

using namespace cache;

AbstractCacheWriter::AbstractCacheWriter(std::ostream& stream)
    : m_stream(stream)
{
}

void AbstractCacheWriter::Write(const void* data, const size_t dataSize) const
{
    m_stream.write(static_cast<const char*>(data), dataSize);
}

void AbstractCacheWriter::WriteString(const char* str) const
{
    if (!str)
        return;

    const auto len = static_cast<uint16_t>(strnlen(str, 0xFFFF));
    Write(&len, sizeof(len));
    Write(str, len);
}

void AbstractCacheWriter::WriteBool(const bool value) const
{
    m_stream << (value ? '\x01' : '\x00');
}

void AbstractCacheWriter::WriteFalse() const
{
    WriteBool(false);
}

void AbstractCacheWriter::WriteTrue() const
{
    WriteBool(true);
}

void AbstractCacheWriter::WriteFileDependency(const std::string& dependencyName, uint64_t timestamp) const {}

void AbstractCacheWriter::WriteGdtDependency(const std::string& dependencyName, uint32_t crc32) const {}
