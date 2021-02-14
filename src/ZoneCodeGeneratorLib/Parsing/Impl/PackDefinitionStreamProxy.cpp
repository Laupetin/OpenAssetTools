#include "PackDefinitionStreamProxy.h"

#include "Parsing/ParsingException.h"

PackDefinitionStreamProxy::PackDefinitionStreamProxy(IParserLineStream* stream)
    : m_stream(stream)
{
}

bool PackDefinitionStreamProxy::MatchPackDirective(const ParserLine& line, unsigned directivePosition)
{
    auto packValue = 0;

    if (!MatchString(line, directivePosition, PRAGMA_PACK_DIRECTIVE, std::char_traits<char>::length(PRAGMA_PACK_DIRECTIVE)))
        return false;

    if (!MatchNextCharacter(line, directivePosition, '('))
        throw ParsingException(CreatePos(line, directivePosition), "Invalid pack directive.");

    bool isPush;
    if (MatchNextString(line, directivePosition, PUSH_KEYWORD, std::char_traits<char>::length(PUSH_KEYWORD)))
        isPush = true;
    else if (MatchNextString(line, directivePosition, POP_KEYWORD, std::char_traits<char>::length(POP_KEYWORD)))
        isPush = false;
    else
        throw ParsingException(CreatePos(line, directivePosition), "Unknown pack directive command.");

    if(isPush)
    {
        if (!MatchNextCharacter(line, directivePosition, ','))
            throw ParsingException(CreatePos(line, directivePosition), "Invalid pack directive.");

        if (!ExtractInteger(line, directivePosition, packValue))
            throw ParsingException(CreatePos(line, directivePosition), "Invalid pack value.");
    }

    if (!MatchNextCharacter(line, directivePosition, ')'))
        throw ParsingException(CreatePos(line, directivePosition), "Invalid pack directive.");

    if(isPush)
        m_current_pack.push(packValue);
    else if (!m_current_pack.empty())
        m_current_pack.pop();

    return true;
}

bool PackDefinitionStreamProxy::MatchDirectives(const ParserLine& line)
{
    unsigned directivePos;

    if (!FindDirective(line, directivePos))
        return false;

    directivePos++;
    return MatchPackDirective(line, directivePos);
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

int PackDefinitionStreamProxy::GetCurrentPack()
{
    return m_current_pack.empty() ? DEFAULT_PACK : m_current_pack.top();
}
