#include "MockParserLineStream.h"

#include <algorithm>

MockParserLineStream::MockParserLineStream(const std::vector<std::string>& lines)
{
    AddIncludeLines(MOCK_FILENAME, lines);
    m_include_positions.emplace_back(MOCK_FILENAME, lines);
}

MockParserLineStream::IncludePos::IncludePos(std::string filename, const std::vector<std::string>& lines)
    : m_filename(std::make_shared<std::string>(std::move(filename))),
      m_lines(lines),
      m_pos(0)
{
}

void MockParserLineStream::AddIncludeLines(const std::string& filename, const std::vector<std::string>& lines)
{
    m_include_lines[filename] = lines;
}

ParserLine MockParserLineStream::NextLine()
{
    while (!m_include_positions.empty())
    {
        auto& currentInclude = m_include_positions.back();

        if (currentInclude.m_pos < currentInclude.m_lines.size())
        {
            const auto line = currentInclude.m_pos++;
            return ParserLine(currentInclude.m_filename, static_cast<int>(line + 1), currentInclude.m_lines[line]);
        }

        m_include_positions.pop_back();
    }

    return ParserLine();
}

bool MockParserLineStream::IncludeFile(const std::string& filename)
{
    const auto foundInclude = m_include_lines.find(filename);
    if (foundInclude == m_include_lines.end())
        return false;

    m_include_positions.emplace_back(foundInclude->first, foundInclude->second);
    return true;
}

void MockParserLineStream::PopCurrentFile()
{
    if (!m_include_positions.empty())
        m_include_positions.pop_back();
}

bool MockParserLineStream::IsOpen() const
{
    return true;
}

bool MockParserLineStream::Eof() const
{
    return !std::any_of(m_include_positions.begin(), m_include_positions.end(), [](const IncludePos& pos)
    {
        return pos.m_pos < pos.m_lines.size();
    });
}
