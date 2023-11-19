#include "IncludingStreamProxy.h"

#include "Parsing/ParsingException.h"

#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

IncludingStreamProxy::IncludingStreamProxy(IParserLineStream* stream)
    : m_stream(stream)
{
}

bool IncludingStreamProxy::ExtractIncludeFilename(const ParserLine& line,
                                                  const unsigned includeDirectivePosition,
                                                  unsigned& filenameStartPosition,
                                                  unsigned& filenameEndPosition)
{
    auto currentPos = includeDirectivePosition;
    bool isDoubleQuotes;

    while (isspace(line.m_line[currentPos]))
    {
        if (currentPos++ >= line.m_line.size())
            throw ParsingException(CreatePos(line, currentPos - 1), INCLUDE_QUOTES_ERROR);
    }

    if (line.m_line[currentPos] == '"')
        isDoubleQuotes = true;
    else if (line.m_line[currentPos] == '<')
        isDoubleQuotes = false;
    else
        throw ParsingException(CreatePos(line, currentPos - 1), INCLUDE_QUOTES_ERROR);

    filenameStartPosition = ++currentPos;
    filenameEndPosition = 0;

    for (; currentPos < line.m_line.size(); currentPos++)
    {
        const auto c = line.m_line[currentPos];

        if (c == '"')
        {
            if (!isDoubleQuotes)
                throw ParsingException(CreatePos(line, currentPos - 1), "");
            filenameEndPosition = currentPos;
            return true;
        }

        if (c == '>')
        {
            if (isDoubleQuotes)
                throw ParsingException(CreatePos(line, currentPos - 1), INCLUDE_QUOTES_ERROR);
            filenameEndPosition = currentPos;
            return true;
        }
    }

    return false;
}

bool IncludingStreamProxy::MatchIncludeDirective(const ParserLine& line, const unsigned directiveStartPos, const unsigned directiveEndPos) const
{
    auto currentPos = directiveStartPos;

    if (directiveEndPos - directiveStartPos != std::char_traits<char>::length(INCLUDE_DIRECTIVE)
        || !MatchString(line, currentPos, INCLUDE_DIRECTIVE, std::char_traits<char>::length(INCLUDE_DIRECTIVE)))
    {
        return false;
    }

    unsigned filenameStart, filenameEnd;

    if (!ExtractIncludeFilename(line, currentPos, filenameStart, filenameEnd))
        throw ParsingException(TokenPos(*line.m_filename, line.m_line_number, static_cast<int>(currentPos)), INCLUDE_QUOTES_ERROR);

    if (filenameEnd <= filenameStart)
        throw ParsingException(CreatePos(line, currentPos), "No filename specified");

    const auto filename = line.m_line.substr(filenameStart, filenameEnd - filenameStart);

    if (!m_stream->IncludeFile(filename))
    {
        std::ostringstream errorStr;
        errorStr << "Could not include file \"" << filename << "\"";
        throw ParsingException(CreatePos(line, currentPos), errorStr.str());
    }

    return true;
}

bool IncludingStreamProxy::MatchPragmaOnceDirective(const ParserLine& line, const unsigned directiveStartPos, const unsigned directiveEndPos)
{
    auto currentPos = directiveStartPos;

    if (directiveEndPos - directiveStartPos != std::char_traits<char>::length(PRAGMA_DIRECTIVE)
        || !MatchString(line, currentPos, PRAGMA_DIRECTIVE, std::char_traits<char>::length(PRAGMA_DIRECTIVE)))
    {
        return false;
    }

    if (!SkipWhitespace(line, currentPos))
        return false;

    if (!MatchString(line, currentPos, ONCE_PRAGMA_COMMAND, std::char_traits<char>::length(ONCE_PRAGMA_COMMAND)))
        return false;

    const auto absolutePath = absolute(fs::path(*line.m_filename));
    const auto absolutePathStr = absolutePath.string();

    const auto existingPath = m_included_files.find(absolutePathStr);
    if (existingPath != m_included_files.end())
        m_stream->PopCurrentFile();
    else
        m_included_files.emplace(absolutePathStr);

    return true;
}

bool IncludingStreamProxy::MatchDirectives(const ParserLine& line)
{
    unsigned directiveStartPos, directiveEndPos;

    if (!FindDirective(line, directiveStartPos, directiveEndPos))
        return false;

    directiveStartPos++;
    return MatchIncludeDirective(line, directiveStartPos, directiveEndPos) || MatchPragmaOnceDirective(line, directiveStartPos, directiveEndPos);
}

ParserLine IncludingStreamProxy::NextLine()
{
    auto line = m_stream->NextLine();

    while (MatchDirectives(line))
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
