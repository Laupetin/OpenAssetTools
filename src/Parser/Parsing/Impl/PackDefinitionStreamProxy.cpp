#include "PackDefinitionStreamProxy.h"

#include "Parsing/ParsingException.h"

PackDefinitionStreamProxy::PackDefinitionStreamProxy(IParserLineStream* stream)
    : m_stream(stream)
{
}

bool PackDefinitionStreamProxy::MatchPackDirective(const ParserLine& line, const size_t directiveStartPosition, const size_t directiveEndPosition)
{
    auto packValue = 0;
    auto currentPosition = directiveStartPosition;

    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(PRAGMA_DIRECTIVE)
        || !MatchString(line, currentPosition, PRAGMA_DIRECTIVE, std::char_traits<char>::length(PRAGMA_DIRECTIVE)))
    {
        return false;
    }

    if (!SkipWhitespace(line, currentPosition))
        return false;

    if (!MatchString(line, currentPosition, PACK_PRAGMA_COMMAND, std::char_traits<char>::length(PACK_PRAGMA_COMMAND)))
        return false;

    if (!SkipWhitespace(line, currentPosition))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid pack directive.");

    if (!MatchNextCharacter(line, currentPosition, '('))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid pack directive.");

    bool isPush;
    if (MatchNextString(line, currentPosition, PUSH_KEYWORD, std::char_traits<char>::length(PUSH_KEYWORD)))
        isPush = true;
    else if (MatchNextString(line, currentPosition, POP_KEYWORD, std::char_traits<char>::length(POP_KEYWORD)))
        isPush = false;
    else
        throw ParsingException(CreatePos(line, currentPosition), "Unknown pack directive command.");

    if (isPush)
    {
        if (!MatchNextCharacter(line, currentPosition, ','))
            throw ParsingException(CreatePos(line, currentPosition), "Invalid pack directive.");

        if (!ExtractInteger(line, currentPosition, packValue))
            throw ParsingException(CreatePos(line, currentPosition), "Invalid pack value.");
    }

    if (!MatchNextCharacter(line, currentPosition, ')'))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid pack directive.");

    if (isPush)
        m_current_pack.push(packValue);
    else if (!m_current_pack.empty())
        m_current_pack.pop();

    return true;
}

bool PackDefinitionStreamProxy::MatchDirectives(const ParserLine& line)
{
    size_t directiveStartPos, directiveEndPos;

    if (!FindDirective(line, directiveStartPos, directiveEndPos))
        return false;

    directiveStartPos++;
    return MatchPackDirective(line, directiveStartPos, directiveEndPos);
}

ParserLine PackDefinitionStreamProxy::NextLine()
{
    auto line = m_stream->NextLine();

    while (MatchDirectives(line))
        line = m_stream->NextLine();

    return line;
}

bool PackDefinitionStreamProxy::IncludeFile(const std::string& filename)
{
    return m_stream->IncludeFile(filename);
}

void PackDefinitionStreamProxy::PopCurrentFile()
{
    m_stream->PopCurrentFile();
}

bool PackDefinitionStreamProxy::IsOpen() const
{
    return m_stream->IsOpen();
}

bool PackDefinitionStreamProxy::Eof() const
{
    return m_stream->Eof();
}

int PackDefinitionStreamProxy::GetCurrentPack() const
{
    return m_current_pack.empty() ? DEFAULT_PACK : m_current_pack.top();
}
