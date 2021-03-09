#include "ParserInputStream.h"

#include <sstream>

ParserInputStream::ParserInputStream(std::istream& stream, std::string fileName)
    : m_stream(stream),
      m_file_name(std::move(fileName)),
      m_line_number(1)
{
}

ParserLine ParserInputStream::NextLine()
{
    std::ostringstream str;
    auto hasLength = false;

    auto c = m_stream.get();
    while (c != EOF)
    {
        switch (c)
        {
        case '\r':
            c = m_stream.get();
            if (c == '\n')
                return ParserLine(m_file_name, m_line_number++, str.str());
            str << '\r';
            hasLength = true;
            continue;

        case '\n':
            return ParserLine(m_file_name, m_line_number++, str.str());

        default:
            str << static_cast<char>(c);
            hasLength = true;
            break;
        }

        c = m_stream.get();
    }

    if (hasLength)
        return ParserLine(m_file_name, m_line_number, str.str());

    return ParserLine();
}

bool ParserInputStream::IncludeFile(const std::string& filename)
{
    return false;
}

void ParserInputStream::PopCurrentFile()
{
}

bool ParserInputStream::IsOpen() const
{
    return !m_stream.eof();
}

bool ParserInputStream::Eof() const
{
    return !m_stream.eof();
}
