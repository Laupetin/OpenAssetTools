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

bool IncludingStreamProxy::MatchIncludeDirective(const ParserLine& line, unsigned directivePosition) const
{
    if (!MatchString(line, directivePosition, INCLUDE_DIRECTIVE, std::char_traits<char>::length(INCLUDE_DIRECTIVE)))
        return false;
    
    unsigned filenameStart, filenameEnd;

    if (!ExtractIncludeFilename(line, directivePosition, filenameStart, filenameEnd))
        throw ParsingException(TokenPos(line.m_filename, line.m_line_number, directivePosition), INCLUDE_QUOTES_ERROR);

    if (filenameEnd <= filenameStart)
        throw ParsingException(CreatePos(line, directivePosition), "No filename specified");

    const auto filename = line.m_line.substr(filenameStart, filenameEnd - filenameStart);

    if (!m_stream->IncludeFile(filename))
    {
        std::ostringstream errorStr;
        errorStr << "Could not include file \"" << filename << "\"";
        throw ParsingException(CreatePos(line, directivePosition), errorStr.str());
    }

    return true;
}

bool IncludingStreamProxy::MatchPragmaOnceDirective(const ParserLine& line, unsigned directivePosition)
{
    if(!MatchString(line, directivePosition, PRAGMA_ONCE_DIRECTIVE, std::char_traits<char>::length(PRAGMA_ONCE_DIRECTIVE)))
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

bool IncludingStreamProxy::MatchDirectives(const ParserLine& line)
{
    unsigned directivePos;

    if (!FindDirective(line, directivePos))
        return false;

    directivePos++;
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
