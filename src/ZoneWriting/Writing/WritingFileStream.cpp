#include "WritingFileStream.h"

WritingFileStream::WritingFileStream(std::ostream& stream)
    : m_stream(stream)
{
}

void WritingFileStream::Write(const void* buffer, const size_t length)
{
    m_stream.write(static_cast<const char*>(buffer), length);
}

void WritingFileStream::Flush()
{
    m_stream.flush();
}

int64_t WritingFileStream::Pos()
{
    return m_stream.tellp();
}
