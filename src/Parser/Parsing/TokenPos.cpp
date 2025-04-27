#include "TokenPos.h"

namespace
{
    const std::string EMPTY_FILENAME;
}

TokenPos::TokenPos()
    : m_filename(EMPTY_FILENAME),
      m_line(1uz),
      m_column(1uz)
{
}

TokenPos::TokenPos(const std::string& filename, const size_t line, const size_t column)
    : m_filename(filename),
      m_line(line),
      m_column(column)
{
}
