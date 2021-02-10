#include "IParserLineStream.h"

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
