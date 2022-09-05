#include "DefinesStreamProxy.h"

#include <sstream>
#include <utility>

#include "Utils/ClassUtils.h"
#include "AbstractParser.h"
#include "ParserSingleInputStream.h"
#include "Defines/DefinesDirectiveParser.h"
#include "Parsing/ParsingException.h"
#include "Parsing/Simple/SimpleLexer.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"
#include "Parsing/Simple/Expression/SimpleExpressionMatchers.h"
#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

DefinesStreamProxy::DefineParameterPosition::DefineParameterPosition()
    : m_parameter_index(0u),
      m_parameter_position(0u)
{
}

DefinesStreamProxy::DefineParameterPosition::DefineParameterPosition(const unsigned index, const unsigned position)
    : m_parameter_index(index),
      m_parameter_position(position)
{
}

DefinesStreamProxy::Define::Define()
= default;

DefinesStreamProxy::Define::Define(std::string name, std::string value)
    : m_name(std::move(name)),
      m_value(std::move(value))
{
}

std::string DefinesStreamProxy::Define::Render(const std::vector<std::string>& parameterValues)
{
    if (parameterValues.empty() || m_parameter_positions.empty())
        return m_value;

    std::ostringstream str;
    auto lastPos = 0u;
    for (const auto& parameterPosition : m_parameter_positions)
    {
        if (lastPos < parameterPosition.m_parameter_position)
            str << std::string(m_value, lastPos, parameterPosition.m_parameter_position - lastPos);

        if (parameterPosition.m_parameter_index < parameterValues.size())
        {
            str << parameterValues[parameterPosition.m_parameter_index];
        }

        lastPos = parameterPosition.m_parameter_position;
    }

    if (lastPos < m_value.size())
        str << std::string(m_value, lastPos, m_value.size() - lastPos);

    return str.str();
}

void DefinesStreamProxy::Define::IdentifyParameters(const std::vector<std::string>& parameterNames)
{
    if (parameterNames.empty())
        return;

    auto inWord = false;
    auto wordStart = 0u;
    for (auto i = 0u; i < m_value.size(); i++)
    {
        const auto c = m_value[i];
        if (!isalnum(c) && c != '_')
        {
            if (inWord)
            {
                const std::string word(m_value, wordStart, i - wordStart);

                auto parameterIndex = 0u;
                for (; parameterIndex < parameterNames.size(); parameterIndex++)
                {
                    if (word == parameterNames[parameterIndex])
                        break;
                }

                if (parameterIndex < parameterNames.size())
                {
                    m_value.erase(wordStart, i - wordStart);
                    m_parameter_positions.emplace_back(parameterIndex, wordStart);
                    i = wordStart;
                }

                inWord = false;
            }
        }
        else
        {
            if (!inWord && (isalpha(c) || c == '_'))
            {
                inWord = true;
                wordStart = i;
            }
        }
    }

    if (inWord)
    {
        const std::string word(m_value, wordStart, m_value.size() - wordStart);

        auto parameterIndex = 0u;
        for (; parameterIndex < parameterNames.size(); parameterIndex++)
        {
            if (word == parameterNames[parameterIndex])
                break;
        }

        if (parameterIndex < parameterNames.size())
        {
            m_value.erase(wordStart, m_value.size() - wordStart);
            m_parameter_positions.emplace_back(parameterIndex, wordStart);
        }
    }
}

DefinesStreamProxy::DefinesStreamProxy(IParserLineStream* stream)
    : m_stream(stream),
      m_ignore_depth(0),
      m_in_define(false)
{
}

int DefinesStreamProxy::GetLineEndEscapePos(const ParserLine& line)
{
    for (auto linePos = line.m_line.size(); linePos > 0; linePos--)
    {
        const auto c = line.m_line[linePos - 1];
        if (c == '\\')
            return static_cast<int>(linePos) - 1;

        if (!isspace(c))
            return -1;
    }

    return -1;
}

void DefinesStreamProxy::ContinueDefine(const ParserLine& line)
{
    const auto lineEndEscapePos = GetLineEndEscapePos(line);
    if (lineEndEscapePos < 0)
    {
        m_current_define_value << line.m_line;
        m_current_define.m_value = m_current_define_value.str();
        m_current_define.IdentifyParameters(m_current_define_parameters);
        AddDefine(std::move(m_current_define));

        m_in_define = false;
        m_current_define = Define();
        m_current_define_value.str(std::string());
        m_current_define_parameters.clear();
    }
    else
    {
        if (line.m_line.size() > static_cast<unsigned>(lineEndEscapePos))
            m_current_define_value << line.m_line.substr(0, static_cast<unsigned>(lineEndEscapePos));
    }
}

std::vector<std::string> DefinesStreamProxy::MatchDefineParameters(const ParserLine& line, unsigned& parameterPosition)
{
    if (line.m_line[parameterPosition] != '(')
        return std::vector<std::string>();

    parameterPosition++;
    std::vector<std::string> parameters;

    while (true)
    {
        if (!SkipWhitespace(line, parameterPosition) || parameterPosition >= line.m_line.size())
            throw ParsingException(CreatePos(line, parameterPosition), "Invalid define parameter list");

        const auto nameStartPos = parameterPosition;
        if (!ExtractIdentifier(line, parameterPosition))
            throw ParsingException(CreatePos(line, parameterPosition), "Cannot extract name of parameter of define");

        parameters.emplace_back(std::string(line.m_line, nameStartPos, parameterPosition - nameStartPos));

        if (!SkipWhitespace(line, parameterPosition))
            throw ParsingException(CreatePos(line, parameterPosition), "Unclosed define parameters");

        if (parameterPosition >= line.m_line.size())
            throw ParsingException(CreatePos(line, parameterPosition), "Unclosed define parameters");

        if (line.m_line[parameterPosition] == ')')
        {
            parameterPosition++;
            break;
        }

        if (line.m_line[parameterPosition] != ',')
            throw ParsingException(CreatePos(line, parameterPosition), "Unknown symbol in define parameter list");

        parameterPosition++;
    }

    return parameters;
}

bool DefinesStreamProxy::MatchDefineDirective(const ParserLine& line, const unsigned directiveStartPosition, const unsigned directiveEndPosition)
{
    auto currentPos = directiveStartPosition;

    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(DEFINE_DIRECTIVE)
        || !MatchString(line, currentPos, DEFINE_DIRECTIVE, std::char_traits<char>::length(DEFINE_DIRECTIVE)))
    {
        return false;
    }

    if (!SkipWhitespace(line, currentPos))
        throw ParsingException(CreatePos(line, currentPos), "Cannot define without a name.");

    const auto nameStartPos = currentPos;
    if (!ExtractIdentifier(line, currentPos))
        throw ParsingException(CreatePos(line, currentPos), "Cannot define without a name.");

    const auto name = line.m_line.substr(nameStartPos, currentPos - nameStartPos);

    auto parameters = MatchDefineParameters(line, currentPos);

    const auto lineEndEscapePos = GetLineEndEscapePos(line);
    if (lineEndEscapePos < 0)
    {
        std::string value;
        if (currentPos < line.m_line.size())
            value = line.m_line.substr(currentPos + 1);

        Define define(name, value);
        define.IdentifyParameters(parameters);
        AddDefine(std::move(define));
    }
    else
    {
        m_in_define = true;
        m_current_define = Define(name, std::string());
        m_current_define_value.str(std::string());
        m_current_define_parameters = std::move(parameters);

        if (currentPos < line.m_line.size() && (currentPos + 1) < static_cast<unsigned>(lineEndEscapePos))
            m_current_define_value << line.m_line.substr(currentPos + 1, static_cast<unsigned>(lineEndEscapePos) - (currentPos + 1));
    }

    return true;
}

bool DefinesStreamProxy::MatchUndefDirective(const ParserLine& line, const unsigned directiveStartPosition, const unsigned directiveEndPosition)
{
    auto currentPos = directiveStartPosition;

    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(UNDEF_DIRECTIVE)
        || !MatchString(line, currentPos, UNDEF_DIRECTIVE, std::char_traits<char>::length(UNDEF_DIRECTIVE)))
    {
        return false;
    }

    if (!SkipWhitespace(line, currentPos))
        throw ParsingException(CreatePos(line, currentPos), "Cannot undef without a name.");

    const auto nameStartPos = currentPos;
    if (!ExtractIdentifier(line, currentPos))
        throw ParsingException(CreatePos(line, currentPos), "Cannot undef without a name.");

    const auto name = line.m_line.substr(nameStartPos, currentPos - nameStartPos);
    const auto entry = m_defines.find(name);

    if (entry != m_defines.end())
        m_defines.erase(entry);

    return true;
}

std::unique_ptr<ISimpleExpression> DefinesStreamProxy::ParseExpression(const std::string& expressionString) const
{
    std::istringstream ss(expressionString);
    ParserSingleInputStream inputStream(ss, "#if expression");

    SimpleLexer::Config lexerConfig;
    lexerConfig.m_emit_new_line_tokens = false;
    lexerConfig.m_read_integer_numbers = true;
    lexerConfig.m_read_floating_point_numbers = true;
    lexerConfig.m_read_strings = false;
    SimpleExpressionMatchers().ApplyTokensToLexerConfig(lexerConfig);

    SimpleLexer lexer(&inputStream, std::move(lexerConfig));
    DefinesDirectiveParser parser(&lexer, m_defines);

    if (!parser.Parse())
        return nullptr;

    return parser.GetParsedExpression();
}

bool DefinesStreamProxy::MatchIfDirective(const ParserLine& line, const unsigned directiveStartPosition, const unsigned directiveEndPosition)
{
    auto currentPos = directiveStartPosition;

    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(IF_DIRECTIVE)
        || !MatchString(line, currentPos, IF_DIRECTIVE, std::char_traits<char>::length(IF_DIRECTIVE)))
    {
        return false;
    }

    if (!m_modes.empty() && m_modes.top() != BlockMode::IN_BLOCK)
    {
        m_ignore_depth++;
        return true;
    }

    if (!SkipWhitespace(line, currentPos))
        throw ParsingException(CreatePos(line, currentPos), "Cannot if without an expression.");

    const auto expressionString = line.m_line.substr(currentPos, line.m_line.size() - currentPos);

    if (expressionString.empty())
        throw ParsingException(CreatePos(line, currentPos), "Cannot if without an expression.");

    const auto expression = ParseExpression(expressionString);
    if (!expression)
        throw ParsingException(CreatePos(line, currentPos), "Failed to parse if expression");

    m_modes.push(expression->EvaluateStatic().IsTruthy() ? BlockMode::IN_BLOCK : BlockMode::NOT_IN_BLOCK);

    return true;
}

bool DefinesStreamProxy::MatchElIfDirective(const ParserLine& line, const unsigned directiveStartPosition, const unsigned directiveEndPosition)
{
    auto currentPos = directiveStartPosition;

    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(ELIF_DIRECTIVE)
        || !MatchString(line, currentPos, ELIF_DIRECTIVE, std::char_traits<char>::length(ELIF_DIRECTIVE)))
    {
        return false;
    }

    if (m_ignore_depth > 0)
        return true;

    if (m_modes.empty())
        throw ParsingException(CreatePos(line, currentPos), "Cannot use elif without if");

    if (m_modes.top() == BlockMode::BLOCK_BLOCKED)
        return true;

    if(m_modes.top() == BlockMode::IN_BLOCK)
    {
        m_modes.top() = BlockMode::BLOCK_BLOCKED;
        return true;
    }

    if (!SkipWhitespace(line, currentPos))
        throw ParsingException(CreatePos(line, currentPos), "Cannot elif without an expression.");

    const auto expressionString = line.m_line.substr(currentPos, line.m_line.size() - currentPos);

    if (expressionString.empty())
        throw ParsingException(CreatePos(line, currentPos), "Cannot elif without an expression.");

    const auto expression = ParseExpression(expressionString);
    if (!expression)
        throw ParsingException(CreatePos(line, currentPos), "Failed to parse elif expression");

    m_modes.top() = expression->EvaluateStatic().IsTruthy() ? BlockMode::IN_BLOCK : BlockMode::NOT_IN_BLOCK;

    return true;
}

bool DefinesStreamProxy::MatchIfdefDirective(const ParserLine& line, const unsigned directiveStartPosition, const unsigned directiveEndPosition)
{
    auto currentPos = directiveStartPosition;

    auto reverse = false;
    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(IFDEF_DIRECTIVE)
        || !MatchString(line, currentPos, IFDEF_DIRECTIVE, std::char_traits<char>::length(IFDEF_DIRECTIVE)))
    {
        if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(IFNDEF_DIRECTIVE)
            || !MatchString(line, currentPos, IFNDEF_DIRECTIVE, std::char_traits<char>::length(IFNDEF_DIRECTIVE)))
        {
            return false;
        }

        reverse = true;
    }

    if (!m_modes.empty() && m_modes.top() != BlockMode::IN_BLOCK)
    {
        m_ignore_depth++;
        return true;
    }

    if (!SkipWhitespace(line, currentPos))
        throw ParsingException(CreatePos(line, currentPos), "Cannot ifdef without a name.");

    const auto nameStartPos = currentPos;
    if (!ExtractIdentifier(line, currentPos))
        throw ParsingException(CreatePos(line, currentPos), "Cannot ifdef without a name.");

    const auto name = line.m_line.substr(nameStartPos, currentPos - nameStartPos);
    const auto entry = m_defines.find(name);

    if (entry != m_defines.end())
        m_modes.push(!reverse ? BlockMode::IN_BLOCK : BlockMode::NOT_IN_BLOCK);
    else
        m_modes.push(reverse ? BlockMode::IN_BLOCK : BlockMode::NOT_IN_BLOCK);

    return true;
}

bool DefinesStreamProxy::MatchElseDirective(const ParserLine& line, const unsigned directiveStartPosition, const unsigned directiveEndPosition)
{
    auto currentPos = directiveStartPosition;

    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(ELSE_DIRECTIVE)
        || !MatchString(line, currentPos, ELSE_DIRECTIVE, std::char_traits<char>::length(ELSE_DIRECTIVE)))
    {
        return false;
    }

    if (m_ignore_depth > 0)
        return true;

    if (m_modes.empty())
        throw ParsingException(CreatePos(line, currentPos), "Cannot use else without ifdef");

    m_modes.top() = m_modes.top() == BlockMode::NOT_IN_BLOCK ? BlockMode::IN_BLOCK : BlockMode::BLOCK_BLOCKED;

    return true;
}

bool DefinesStreamProxy::MatchEndifDirective(const ParserLine& line, const unsigned directiveStartPosition, const unsigned directiveEndPosition)
{
    auto currentPos = directiveStartPosition;

    if (directiveEndPosition - directiveStartPosition != std::char_traits<char>::length(ENDIF_DIRECTIVE)
        || !MatchString(line, currentPos, ENDIF_DIRECTIVE, std::char_traits<char>::length(ENDIF_DIRECTIVE)))
    {
        return false;
    }

    if (m_ignore_depth > 0)
    {
        m_ignore_depth--;
        return true;
    }

    if (!m_modes.empty())
        m_modes.pop();
    else
        throw ParsingException(CreatePos(line, currentPos), "Cannot use endif without ifdef");

    return true;
}

bool DefinesStreamProxy::MatchDirectives(const ParserLine& line)
{
    unsigned directiveStartPos;
    unsigned directiveEndPos;

    if (!FindDirective(line, directiveStartPos, directiveEndPos))
        return false;

    directiveStartPos++;

    if (m_modes.empty() || m_modes.top() == BlockMode::IN_BLOCK)
    {
        if (MatchDefineDirective(line, directiveStartPos, directiveEndPos)
            || MatchUndefDirective(line, directiveStartPos, directiveEndPos))
            return true;
    }

    return MatchIfdefDirective(line, directiveStartPos, directiveEndPos)
        || MatchIfDirective(line, directiveStartPos, directiveEndPos)
        || MatchElIfDirective(line, directiveStartPos, directiveEndPos)
        || MatchElseDirective(line, directiveStartPos, directiveEndPos)
        || MatchEndifDirective(line, directiveStartPos, directiveEndPos);
}

bool DefinesStreamProxy::FindDefineForWord(const ParserLine& line, const unsigned wordStart, const unsigned wordEnd, Define*& value)
{
    const std::string word(line.m_line, wordStart, wordEnd - wordStart);
    const auto foundEntry = m_defines.find(word);
    if (foundEntry != m_defines.end())
    {
        value = &foundEntry->second;
        return true;
    }

    return false;
}

void DefinesStreamProxy::ExtractParametersFromDefineUsage(const ParserLine& line, const unsigned parameterStart, unsigned& parameterEnd, std::vector<std::string>& parameterValues)
{
    if (line.m_line[parameterStart] != '(')
        return;

    std::ostringstream currentValue;
    auto pos = parameterStart + 1;
    auto valueHasStarted = false;
    auto parenthesisDepth = 0;
    while (true)
    {
        if (pos >= line.m_line.size())
            throw ParsingException(CreatePos(line, pos), "Invalid use of define");

        const auto c = line.m_line[pos];

        if (c == ',')
        {
            if (parenthesisDepth > 0)
            {
                valueHasStarted = true;
                currentValue << c;
            }
            else
            {
                parameterValues.emplace_back(currentValue.str());
                currentValue.clear();
                currentValue.str(std::string());
                valueHasStarted = false;
            }
        }
        else if(c == '(')
        {
            valueHasStarted = true;
            parenthesisDepth++;
            currentValue << c;
        }
        else if (c == ')')
        {
            if(parenthesisDepth > 0)
            {
                valueHasStarted = true;
                parenthesisDepth--;
                currentValue << c;
            }
            else
            {
                parameterValues.emplace_back(currentValue.str());
                parameterEnd = pos + 1;
                break;
            }
        }
        else if (valueHasStarted || !isspace(c))
        {
            valueHasStarted = true;
            currentValue << c;
        }

        pos++;
    }
}

void DefinesStreamProxy::ExpandDefines(ParserLine& line)
{
    bool usesDefines;
    auto defineIterations = 0u;

    do
    {
        if (defineIterations > MAX_DEFINE_ITERATIONS)
            throw ParsingException(CreatePos(line, 1), "Potential define loop? Exceeded max define iterations of " + std::to_string(MAX_DEFINE_ITERATIONS) + " iterations.");

        usesDefines = false;

        auto wordStart = 0u;
        auto lastWordEnd = 0u;
        auto inWord = false;
        Define* value;
        std::ostringstream str;

        for (auto i = 0u; i < line.m_line.size(); i++)
        {
            const auto c = line.m_line[i];
            if (!inWord)
            {
                if (isalpha(c) || c == '_')
                {
                    wordStart = i;
                    inWord = true;
                }
            }
            else
            {
                if (!isalnum(c) && c != '_')
                {
                    if (FindDefineForWord(line, wordStart, i, value))
                    {
                        std::vector<std::string> parameterValues;
                        ExtractParametersFromDefineUsage(line, i, i, parameterValues);
                        const auto defineValue = value->Render(parameterValues);

                        if (!usesDefines)
                        {
                            str << std::string(line.m_line, 0, wordStart) << defineValue;
                            usesDefines = true;
                        }
                        else
                        {
                            str << std::string(line.m_line, lastWordEnd, wordStart - lastWordEnd) << defineValue;
                        }
                        lastWordEnd = i;
                    }
                    inWord = false;
                }
            }
        }

        if (inWord)
        {
            if (FindDefineForWord(line, wordStart, line.m_line.size(), value))
            {
                const std::vector<std::string> parameterValues;
                const auto defineValue = value->Render(parameterValues);

                if (!usesDefines)
                {
                    str << std::string(line.m_line, 0, wordStart) << defineValue;
                    usesDefines = true;
                }
                else
                {
                    str << std::string(line.m_line, lastWordEnd, wordStart - lastWordEnd) << defineValue;
                }
                lastWordEnd = line.m_line.size();
            }
        }

        if (usesDefines)
        {
            if (lastWordEnd < line.m_line.size())
                str << std::string(line.m_line, lastWordEnd, line.m_line.size() - lastWordEnd);
            line.m_line = str.str();
        }

        defineIterations++;
    }
    while (usesDefines);
}

void DefinesStreamProxy::AddDefine(Define define)
{
    m_defines[define.m_name] = std::move(define);
}

void DefinesStreamProxy::Undefine(const std::string& name)
{
    const auto entry = m_defines.find(name);

    if (entry != m_defines.end())
        m_defines.erase(entry);
}

ParserLine DefinesStreamProxy::NextLine()
{
    auto line = m_stream->NextLine();

    if (m_in_define)
    {
        ContinueDefine(line);
        line.m_line.clear();
    }
    else if (MatchDirectives(line) || !m_modes.empty() && m_modes.top() != BlockMode::IN_BLOCK)
    {
        line.m_line.clear();
    }
    else
    {
        ExpandDefines(line);
    }

    return line;
}

bool DefinesStreamProxy::IncludeFile(const std::string& filename)
{
    return m_stream->IncludeFile(filename);
}

void DefinesStreamProxy::PopCurrentFile()
{
    m_stream->PopCurrentFile();
}

bool DefinesStreamProxy::IsOpen() const
{
    return m_stream->IsOpen();
}

bool DefinesStreamProxy::Eof() const
{
    return m_stream->Eof();
}
