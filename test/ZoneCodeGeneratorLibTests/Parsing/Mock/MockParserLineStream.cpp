#include "MockParserLineStream.h"

const std::string MockParserLineStream::MOCK_FILENAME;

MockParserLineStream::MockParserLineStream(const std::vector<std::string>& lines)
    : m_lines(lines),
      m_line(0)
{
}

ParserLine MockParserLineStream::NextLine()
{
    if(m_line < m_lines.size())
    {
        const auto line = m_line++;
        return ParserLine(MOCK_FILENAME, line + 1, m_lines[line]);
    }

    return ParserLine(MOCK_FILENAME, 0, std::string());
}

bool MockParserLineStream::IncludeFile(const std::string& filename)
{
    m_includes.push_back(filename);
    return true;
}

bool MockParserLineStream::IsOpen() const
{
    return true;
}

bool MockParserLineStream::Eof() const
{
    return m_line >= m_lines.size();
}
