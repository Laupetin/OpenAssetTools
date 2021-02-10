#include "TokenPos.h"

const std::string TokenPos::EMPTY_FILENAME;

TokenPos::TokenPos()
    : m_filename(EMPTY_FILENAME),
      m_line(1),
      m_column(1)
{
}

TokenPos::TokenPos(const std::string& filename, const int line, const int column)
    : m_filename(filename),
      m_line(line),
      m_column(column)
{
}
