#include "IncludingStreamProxy.h"

#include <sstream>

#include "Parsing/ParsingException.h"

IncludingStreamProxy::IncludingStreamProxy(IParserLineStream* stream)
    : m_stream(stream)
{
}

bool IncludingStreamProxy::MatchIncludeDirective(const ParserLine& line) const
{
    unsigned includeDirectivePos = 0;
    auto hasIncludeDirective = false;

    for (; includeDirectivePos < line.m_line.size() - INCLUDE_DIRECTIVE_MINIMUM_TOTAL_LENGTH; includeDirectivePos++)
    {
        const auto c = line.m_line[includeDirectivePos];

        if (isspace(c))
            continue;

        if (c != '#')
            return false;

        if (line.m_line.compare(includeDirectivePos + 1, INCLUDE_DIRECTIVE_LENGTH, INCLUDE_DIRECTIVE) != 0)
        {
            return false;
        }

        hasIncludeDirective = true;
        break;
    }

    if (!hasIncludeDirective)
        return false;

    auto currentPos = includeDirectivePos + INCLUDE_DIRECTIVE_LENGTH + 1;
    bool isDoubleQuotes;
    if (line.m_line[currentPos] == '"')
        isDoubleQuotes = true;
    else if (line.m_line[currentPos] == '<')
        isDoubleQuotes = false;
    else
        throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), INCLUDE_QUOTES_ERROR);

    const auto filenameStart = ++currentPos;
    unsigned filenameEnd = 0;
    auto filenameHasEnd = false;

    for (; currentPos < line.m_line.size(); currentPos++)
    {
        const auto c = line.m_line[currentPos];

        if (c == '"')
        {
            if(!isDoubleQuotes)
                throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), "");
            filenameEnd = currentPos;
            filenameHasEnd = true;
            break;
        }

        if (c == '>')
        {
            if (isDoubleQuotes)
                throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), INCLUDE_QUOTES_ERROR);
            filenameEnd = currentPos;
            filenameHasEnd = true;
            break;
        }
    }

    if(!filenameHasEnd)
        throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), INCLUDE_QUOTES_ERROR);

    if(filenameEnd <= filenameStart)
        throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), "No filename specified");

    const auto filename = line.m_line.substr(filenameStart, filenameEnd - filenameStart);

    if(!m_stream->IncludeFile(filename))
    {
        std::ostringstream errorStr;
        errorStr << "Could not include file \"" << filename << "\"";
        throw ParsingException(TokenPos(line.m_filename, line.m_line_number, currentPos), errorStr.str());
    }

    return true;
}

ParserLine IncludingStreamProxy::NextLine()
{
    auto line = m_stream->NextLine();

    if (MatchIncludeDirective(line))
        return m_stream->NextLine();

    return line;
}

bool IncludingStreamProxy::IncludeFile(const std::string& filename)
{
    return m_stream->IncludeFile(filename);
}

bool IncludingStreamProxy::IsOpen() const
{
    return m_stream->IsOpen();
}

bool IncludingStreamProxy::Eof() const
{
    return m_stream->Eof();
}
