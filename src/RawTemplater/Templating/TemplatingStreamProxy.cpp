#include "TemplatingStreamProxy.h"

#include <iostream>

#include "Parsing/ParsingException.h"

using namespace templating;

TemplatingStreamProxy::TemplatingStreamProxy(IParserLineStream* stream, ITemplaterControl* templaterControl)
    : m_stream(stream),
      m_templater_control(templaterControl),
      m_defines_proxy(nullptr)
{
}

void TemplatingStreamProxy::SetDefinesProxy(DefinesStreamProxy* definesProxy)
{
    m_defines_proxy = definesProxy;
}

bool TemplatingStreamProxy::MatchSwitchDirective(const ParserLine& line, const unsigned directiveStartPosition, const unsigned directiveEndPosition) const
{
    auto currentPosition = directiveStartPosition;

    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(PRAGMA_DIRECTIVE)
        || !MatchString(line, currentPosition, PRAGMA_DIRECTIVE, std::char_traits<char>::length(PRAGMA_DIRECTIVE)))
    {
        return false;
    }

    if (!SkipWhitespace(line, currentPosition))
        return false;

    if (!MatchString(line, currentPosition, SWITCH_PRAGMA_COMMAND, std::char_traits<char>::length(SWITCH_PRAGMA_COMMAND)))
        return false;

    if (!SkipWhitespace(line, currentPosition))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid switch directive.");

    const auto nameStartPosition = currentPosition;
    if (!ExtractIdentifier(line, currentPosition))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid switch directive.");

    auto name = line.m_line.substr(nameStartPosition, currentPosition - nameStartPosition);
    
    m_templater_control->AddSwitch(std::move(name));
    return true;
}

bool TemplatingStreamProxy::MatchOptionsDirective(const ParserLine& line, const unsigned directiveStartPosition, const unsigned directiveEndPosition) const
{
    auto currentPosition = directiveStartPosition;

    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(PRAGMA_DIRECTIVE)
        || !MatchString(line, currentPosition, PRAGMA_DIRECTIVE, std::char_traits<char>::length(PRAGMA_DIRECTIVE)))
    {
        return false;
    }

    if (!SkipWhitespace(line, currentPosition))
        return false;

    if (!MatchString(line, currentPosition, OPTIONS_PRAGMA_COMMAND, std::char_traits<char>::length(OPTIONS_PRAGMA_COMMAND)))
        return false;

    if (!SkipWhitespace(line, currentPosition))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid options directive.");

    const auto nameStartPosition = currentPosition;
    if (!ExtractIdentifier(line, currentPosition))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid options directive.");

    auto name = line.m_line.substr(nameStartPosition, currentPosition - nameStartPosition);

    if (!MatchNextCharacter(line, currentPosition, '('))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid options directive.");

    std::vector<std::string> options;

    if (!SkipWhitespace(line, currentPosition))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid options directive.");

    bool firstArg = true;
    while (!MatchNextCharacter(line, currentPosition, ')'))
    {
        if (!firstArg && !MatchNextCharacter(line, currentPosition, ','))
            throw ParsingException(CreatePos(line, currentPosition), "Invalid options directive.");
        if (!SkipWhitespace(line, currentPosition))
            throw ParsingException(CreatePos(line, currentPosition), "Invalid options directive.");

        const auto optionStartPosition = currentPosition;
        if (!ExtractIdentifier(line, currentPosition))
            throw ParsingException(CreatePos(line, currentPosition), "Invalid options directive.");

        std::ostringstream optionValueBuilder;
        optionValueBuilder << '"' << line.m_line.substr(optionStartPosition, currentPosition - optionStartPosition) << '"';
        options.emplace_back(optionValueBuilder.str());

        firstArg = false;
    }

    m_templater_control->AddOptions(std::move(name), std::move(options));
    return true;
}

bool TemplatingStreamProxy::MatchFilenameDirective(const ParserLine& line, const unsigned directiveStartPosition, const unsigned directiveEndPosition) const
{
    auto currentPosition = directiveStartPosition;

    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(PRAGMA_DIRECTIVE)
        || !MatchString(line, currentPosition, PRAGMA_DIRECTIVE, std::char_traits<char>::length(PRAGMA_DIRECTIVE)))
    {
        return false;
    }

    if (!SkipWhitespace(line, currentPosition))
        return false;

    if (!MatchString(line, currentPosition, FILENAME_PRAGMA_COMMAND, std::char_traits<char>::length(FILENAME_PRAGMA_COMMAND)))
        return false;

    if (!SkipWhitespace(line, currentPosition))
        throw ParsingException(CreatePos(line, currentPosition), "Invalid options directive.");

    const auto expressionString = line.m_line.substr(currentPosition, line.m_line.size() - currentPosition);
    if (expressionString.empty())
        throw ParsingException(CreatePos(line, currentPosition), "Cannot pragma filename without an expression.");

    ParserLine expressionStringAsLine(line.m_filename, line.m_line_number, expressionString);
    m_defines_proxy->ExpandDefines(expressionStringAsLine);

    const auto expression = m_defines_proxy->ParseExpression(expressionStringAsLine.m_line);
    if (!expression)
        throw ParsingException(CreatePos(line, currentPosition), "Failed to parse pragma filename expression");

    if (!expression->IsStatic())
        throw ParsingException(CreatePos(line, currentPosition), "pragma filename expression must be static");

    const auto value = expression->EvaluateStatic();

    if (value.m_type != SimpleExpressionValue::Type::STRING)
        throw ParsingException(CreatePos(line, currentPosition), "pragma filename expression must evaluate to string");
    
    m_templater_control->SetFileName(*value.m_string_value);
    return true;
}

bool TemplatingStreamProxy::MatchDirectives(const ParserLine& line)
{
    unsigned directiveStartPos, directiveEndPos;

    if (!FindDirective(line, directiveStartPos, directiveEndPos))
        return false;

    directiveStartPos++;

    return MatchSwitchDirective(line, directiveStartPos, directiveEndPos)
        || MatchOptionsDirective(line, directiveStartPos, directiveEndPos)
        || MatchFilenameDirective(line, directiveStartPos, directiveEndPos);
}

ParserLine TemplatingStreamProxy::NextLine()
{
    auto line = m_stream->NextLine();

    while (MatchDirectives(line))
        line = m_stream->NextLine();

    return line;
}

bool TemplatingStreamProxy::IncludeFile(const std::string& filename)
{
    return m_stream->IncludeFile(filename);
}

void TemplatingStreamProxy::PopCurrentFile()
{
    m_stream->PopCurrentFile();
}

bool TemplatingStreamProxy::IsOpen() const
{
    return m_stream->IsOpen();
}

bool TemplatingStreamProxy::Eof() const
{
    return m_stream->Eof();
}
