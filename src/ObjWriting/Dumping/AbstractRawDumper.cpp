#include "AbstractRawDumper.h"

AbstractRawDumper::AbstractRawDumper(std::ostream& stream)
    : m_stream(stream),
      m_current_offset(0u)
{
}

size_t AbstractRawDumper::OffsetAdd(const size_t len)
{
    const auto lastOffset = m_current_offset;
    m_current_offset += len;

    return lastOffset;
}

size_t AbstractRawDumper::OffsetAdd(const std::string& str)
{
    return OffsetAdd(str.size() + 1);
}

void AbstractRawDumper::Write(const void* ptr, const size_t size) const
{
    m_stream.write(static_cast<const char*>(ptr), size);
}

void AbstractRawDumper::Write(const std::string& str) const
{
    m_stream.write(str.c_str(), str.size() + 1);
}
