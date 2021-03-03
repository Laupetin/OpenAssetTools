#include "LoadingFileStream.h"

LoadingFileStream::LoadingFileStream(std::istream& stream)
    : m_stream(stream)
{
}

size_t LoadingFileStream::Load(void* buffer, const size_t length)
{
    m_stream.read(static_cast<char*>(buffer), length);
    return static_cast<size_t>(m_stream.gcount());
}

int64_t LoadingFileStream::Pos()
{
    return m_stream.tellg();
}