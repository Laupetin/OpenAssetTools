#include "IParserLineStream.h"

#include <string_view>

namespace
{
    // A UTF-8 BOM is an encoding signature and is only valid at the start of a file.
    constexpr std::string_view UTF8_BOM("\xEF\xBB\xBF", 3);
} // namespace

ParserLine::ParserLine()
    : m_line_number(0)
{
}

ParserLine::ParserLine(std::shared_ptr<std::string> filename, const int lineNumber, std::string line)
    : m_filename(std::move(filename)),
      m_line_number(lineNumber),
      m_line(std::move(line))
{
    if (m_line_number == 1 && m_line.starts_with(UTF8_BOM))
        m_line.erase(0, UTF8_BOM.size());
}

bool ParserLine::IsEof() const
{
    return m_line_number <= 0;
}
