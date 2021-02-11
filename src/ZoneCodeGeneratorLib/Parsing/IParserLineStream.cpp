#include "IParserLineStream.h"

const std::string ParserLine::EMPTY_STRING;

ParserLine::ParserLine()
    : m_filename(EMPTY_STRING),
      m_line_number(0)
{
}

ParserLine::ParserLine(const std::string& filename, const int lineNumber, std::string line)
    : m_filename(filename),
      m_line_number(lineNumber),
      m_line(std::move(line))
{
}

bool ParserLine::IsEof() const
{
    return m_line_number <= 0;
}
