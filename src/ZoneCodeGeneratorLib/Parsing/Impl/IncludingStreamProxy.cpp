#include "IncludingStreamProxy.h"

#include <sstream>
#include <filesystem>

#include "Parsing/ParsingException.h"

namespace fs = std::filesystem;

IncludingStreamProxy::IncludingStreamProxy(IParserLineStream* stream)
    : m_stream(stream)
{
}

bool IncludingStreamProxy::ExtractIncludeFilename(const ParserLine& line, const unsigned includeDirectivePosition, unsigned& filenameStartPosition, unsigned& filenameEndPosition)
{
    auto currentPos = includeDirectivePosition;
    bool isDoubleQuotes;

    while (isspace(line.m_line[currentPos]))
    {
        if (currentPos++ >= line.m_line.size())
            throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), INCLUDE_QUOTES_ERROR);
    }

    if (line.m_line[currentPos] == '"')
        isDoubleQuotes = true;
    else if (line.m_line[currentPos] == '<')
        isDoubleQuotes = false;
    else
        throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), INCLUDE_QUOTES_ERROR);

    filenameStartPosition = ++currentPos;
    filenameEndPosition = 0;

    for (; currentPos < line.m_line.size(); currentPos++)
    {
        const auto c = line.m_line[currentPos];

        if (c == '"')
        {
            if (!isDoubleQuotes)
                throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), "");
            filenameEndPosition = currentPos;
            return true;
        }

        if (c == '>')
        {
            if (isDoubleQuotes)
                throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), INCLUDE_QUOTES_ERROR);
            filenameEndPosition = currentPos;
            return true;
        }
    }

    return false;
}

bool IncludingStreamProxy::MatchIncludeDirective(const ParserLine& line, const unsigned directivePosition) const
{
    constexpr auto directiveLength = std::char_traits<char>::length(INCLUDE_DIRECTIVE);
    if (line.m_line.compare(directivePosition + 1, directiveLength, INCLUDE_DIRECTIVE) != 0)
        return false;

    const auto currentPos = directivePosition + directiveLength + 1;
    unsigned filenameStart, filenameEnd;

    if (!ExtractIncludeFilename(line, currentPos, filenameStart, filenameEnd))
        throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), INCLUDE_QUOTES_ERROR);

    if (filenameEnd <= filenameStart)
        throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), "No filename specified");

    const auto filename = line.m_line.substr(filenameStart, filenameEnd - filenameStart);

    if (!m_stream->IncludeFile(filename))
    {
        std::ostringstream errorStr;
        errorStr << "Could not include file \"" << filename << "\"";
        throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), errorStr.str());
    }

    return true;
}

bool IncludingStreamProxy::MatchPragmaOnceDirective(const ParserLine& line, const unsigned directivePosition)
{
    constexpr auto directiveLength = std::char_traits<char>::length(PRAGMA_ONCE_DIRECTIVE);
    if (line.m_line.compare(directivePosition + 1, directiveLength, PRAGMA_ONCE_DIRECTIVE) != 0)
        return false;

    const auto absolutePath = absolute(fs::path(line.m_filename.get()));
    const auto absolutePathStr = absolutePath.string();

    const auto existingPath = m_included_files.find(absolutePathStr);
    if (existingPath != m_included_files.end())
        m_stream->PopCurrentFile();
    else
        m_included_files.emplace(absolutePathStr);

    return true;
}

bool IncludingStreamProxy::FindDirective(const ParserLine& line, unsigned& directivePosition)
{
    directivePosition = 0;
    for (; directivePosition < line.m_line.size(); directivePosition++)
    {
        const auto c = line.m_line[directivePosition];

        if (isspace(c))
            continue;

        return c == '#';
    }

    return false;
}

bool IncludingStreamProxy::MatchDirectives(const ParserLine& line)
{
    unsigned directivePos;

    if (!FindDirective(line, directivePos))
        return false;

    return MatchIncludeDirective(line, directivePos)
        || MatchPragmaOnceDirective(line, directivePos);
}

ParserLine IncludingStreamProxy::NextLine()
{
    auto line = m_stream->NextLine();

    while(MatchDirectives(line))
        line = m_stream->NextLine();

    return line;
}

bool IncludingStreamProxy::IncludeFile(const std::string& filename)
{
    return m_stream->IncludeFile(filename);
}

void IncludingStreamProxy::PopCurrentFile()
{
    m_stream->PopCurrentFile();
}

bool IncludingStreamProxy::IsOpen() const
{
    return m_stream->IsOpen();
}

bool IncludingStreamProxy::Eof() const
{
    return m_stream->Eof();
}
