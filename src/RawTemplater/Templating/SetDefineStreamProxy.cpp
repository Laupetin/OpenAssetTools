#include "SetDefineStreamProxy.h"

#include "Parsing/ParsingException.h"

using namespace templating;

SetDefineStreamProxy::SetDefineStreamProxy(IParserLineStream* stream)
    : m_stream(stream),
      m_defines_proxy(nullptr)
{
}

void SetDefineStreamProxy::SetDefinesProxy(DefinesStreamProxy* definesProxy)
{
    m_defines_proxy = definesProxy;
}

bool SetDefineStreamProxy::MatchSetDirective(const ParserLine& line, const size_t directiveStartPosition, const size_t directiveEndPosition) const
{
    auto currentPosition = directiveStartPosition;

    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(SET_DIRECTIVE)
        || !MatchString(line, currentPosition, SET_DIRECTIVE, std::char_traits<char>::length(SET_DIRECTIVE)))
    {
        return false;
    }

    if (!SkipWhitespace(line, currentPosition))
        return false;

    const auto nameStartPosition = currentPosition;
    if (!ExtractIdentifier(line, currentPosition))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid set directive.");

    auto name = line.m_line.substr(nameStartPosition, currentPosition - nameStartPosition);

    if (!SkipWhitespace(line, currentPosition))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid set directive.");

    const auto expressionString = line.m_line.substr(currentPosition, line.m_line.size() - currentPosition);
    if (expressionString.empty())
        throw ParsingException(CreatePos(line, currentPosition), "Cannot set without an expression.");

    const auto expression = m_defines_proxy->ParseExpression(line.m_filename, line.m_line_number, expressionString);
    if (!expression)
        throw ParsingException(CreatePos(line, currentPosition), "Failed to parse set expression");

    if (!expression->IsStatic())
        throw ParsingException(CreatePos(line, currentPosition), "set expression must be static");

    const auto value = expression->EvaluateStatic();

    if (value.m_type != SimpleExpressionValue::Type::STRING)
        throw ParsingException(CreatePos(line, currentPosition), "set expression must evaluate to string");

    m_defines_proxy->AddDefine(DefinesStreamProxy::Define(std::move(name), *value.m_string_value));
    return true;
}

bool SetDefineStreamProxy::MatchDirectives(const ParserLine& line) const
{
    size_t directiveStartPos, directiveEndPos;

    if (!FindDirective(line, directiveStartPos, directiveEndPos))
        return false;

    directiveStartPos++;

    return MatchSetDirective(line, directiveStartPos, directiveEndPos);
}

ParserLine SetDefineStreamProxy::NextLine()
{
    auto line = m_stream->NextLine();

    while (MatchDirectives(line))
        line = m_stream->NextLine();

    return line;
}

bool SetDefineStreamProxy::IncludeFile(const std::string& filename)
{
    return m_stream->IncludeFile(filename);
}

void SetDefineStreamProxy::PopCurrentFile()
{
    m_stream->PopCurrentFile();
}

bool SetDefineStreamProxy::IsOpen() const
{
    return m_stream->IsOpen();
}

bool SetDefineStreamProxy::Eof() const
{
    return m_stream->Eof();
}
