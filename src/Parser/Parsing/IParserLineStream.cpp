#include "IParserLineStream.h"

ParserLine::ParserLine()
    : m_line_number(0)
{
}

ParserLine::ParserLine(std::shared_ptr<std::string> filename, const int lineNumber, std::string line)
    : m_filename(std::move(filename)),
      m_line_number(lineNumber),
      m_line(std::move(line))
{
}

bool ParserLine::IsEof() const
{
    return m_line_number <= 0;
}
