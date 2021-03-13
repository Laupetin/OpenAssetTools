#include "ISearchPath.h"

bool SearchPathOpenFile::IsOpen() const
{
    return m_stream != nullptr;
}

SearchPathOpenFile::SearchPathOpenFile()
    : m_length(0)
{
}

SearchPathOpenFile::SearchPathOpenFile(std::unique_ptr<std::istream> stream, const int64_t length)
    : m_stream(std::move(stream)),
      m_length(length)
{
}
