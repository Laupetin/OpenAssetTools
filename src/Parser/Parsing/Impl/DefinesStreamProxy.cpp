#include "DefinesStreamProxy.h"

#include "ParserSingleInputStream.h"
#include "Parsing/ParsingException.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"
#include "Parsing/Simple/Expression/SimpleExpressionMatchers.h"
#include "Parsing/Simple/SimpleExpressionInterpreter.h"
#include "Utils/ClassUtils.h"
#include "Utils/StringUtils.h"

#include <regex>
#include <sstream>
#include <utility>

DefinesStreamProxy::DefineParameterPosition::DefineParameterPosition()
    : m_parameter_index(0u),
      m_parameter_position(0u),
      m_stringize(false)
{
}

DefinesStreamProxy::DefineParameterPosition::DefineParameterPosition(const unsigned index, const unsigned position, const bool stringize)
    : m_parameter_index(index),
      m_parameter_position(position),
      m_stringize(stringize)
{
}

DefinesStreamProxy::Define::Define() = default;

DefinesStreamProxy::Define::Define(std::string name, std::string value)
    : m_name(std::move(name)),
      m_value(std::move(value))
{
}

std::string DefinesStreamProxy::Define::Render(const std::vector<std::string>& parameterValues) const
{
    if (parameterValues.empty() || m_parameter_positions.empty())
        return m_value;

    std::ostringstream str;
    auto lastPos = 0u;
    for (const auto& parameterPosition : m_parameter_positions)
    {
        const auto stringizeSkipLastChar = parameterPosition.m_stringize ? 1 : 0;
        if (lastPos < parameterPosition.m_parameter_position)
            str << std::string(m_value, lastPos, parameterPosition.m_parameter_position - lastPos - stringizeSkipLastChar);

        if (parameterPosition.m_parameter_index < parameterValues.size())
        {
            if (parameterPosition.m_stringize)
                str << '"' << utils::EscapeStringForQuotationMarks(parameterValues[parameterPosition.m_parameter_index]) << '"';
            else
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
                    const auto stringize =
                        // Check if # is prepended to the word
                        (wordStart > 0 && m_value[wordStart - 1] == '#')
                        // Make sure it's not a token pasting operator
                        && (wordStart <= 1 || m_value[wordStart - 2] != '#');

                    m_value.erase(wordStart, i - wordStart);
                    m_parameter_positions.emplace_back(parameterIndex, wordStart, stringize);
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
            const auto stringize =
                // Check if # is prepended to the word
                (wordStart > 0 && m_value[wordStart - 1] == '#')
                // Make sure it's not a token pasting operator
                && (wordStart <= 1 || m_value[wordStart - 2] != '#');

            m_value.erase(wordStart, m_value.size() - wordStart);
            m_parameter_positions.emplace_back(parameterIndex, wordStart, stringize);
        }
    }
}

DefinesStreamProxy::DefinesStreamProxy(IParserLineStream* stream, const bool skipDirectiveLines)
    : m_stream(stream),
      m_skip_directive_lines(skipDirectiveLines),
      m_ignore_depth(0),
      m_in_define(false),
      m_parameter_state(ParameterState::NOT_IN_PARAMETERS),
      m_current_macro(nullptr),
      m_macro_parameter_state(ParameterState::NOT_IN_PARAMETERS)
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

void DefinesStreamProxy::ContinueDefine(const ParserLine& line, const unsigned currentPos)
{
    const auto lineEndEscapePos = GetLineEndEscapePos(line);
    if (lineEndEscapePos < 0)
    {
        if (m_parameter_state != ParameterState::NOT_IN_PARAMETERS)
            throw ParsingException(CreatePos(line, currentPos), "Unclosed macro parameters");

        if (currentPos <= 0)
            m_current_define_value << line.m_line;
        else
            m_current_define_value << line.m_line.substr(currentPos);

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
        if (line.m_line.size() > static_cast<unsigned>(lineEndEscapePos) && currentPos < static_cast<unsigned>(lineEndEscapePos))
            m_current_define_value << line.m_line.substr(currentPos, static_cast<unsigned>(lineEndEscapePos) - currentPos);
    }
}

void DefinesStreamProxy::ContinueParameters(const ParserLine& line, unsigned& currentPos)
{
    const auto lineEndEscapePos = GetLineEndEscapePos(line);
    while (true)
    {
        if (!SkipWhitespace(line, currentPos))
            throw ParsingException(CreatePos(line, currentPos), "Invalid define parameter list");

        if (lineEndEscapePos >= 0 && currentPos >= static_cast<unsigned>(lineEndEscapePos))
            return;

        if (currentPos >= line.m_line.size())
            throw ParsingException(CreatePos(line, currentPos), "Invalid define parameter list");

        if ((m_parameter_state == ParameterState::AFTER_OPEN || m_parameter_state == ParameterState::AFTER_PARAM) && line.m_line[currentPos] == ')')
        {
            currentPos++;
            m_parameter_state = ParameterState::NOT_IN_PARAMETERS;
            return;
        }

        if (m_parameter_state == ParameterState::AFTER_PARAM && line.m_line[currentPos] == ',')
        {
            currentPos++;
            m_parameter_state = ParameterState::AFTER_COMMA;
            continue;
        }

        const auto nameStartPos = currentPos;
        if (!ExtractIdentifier(line, currentPos))
            throw ParsingException(CreatePos(line, currentPos), "Cannot extract name of parameter of define");

        m_current_define_parameters.emplace_back(line.m_line, nameStartPos, currentPos - nameStartPos);
        m_parameter_state = ParameterState::AFTER_PARAM;
    }
}

void DefinesStreamProxy::MatchDefineParameters(const ParserLine& line, unsigned& currentPos)
{
    m_current_define_parameters = std::vector<std::string>();
    if (line.m_line[currentPos] != '(')
        return;

    m_parameter_state = ParameterState::AFTER_OPEN;
    currentPos++;

    ContinueParameters(line, currentPos);
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

    MatchDefineParameters(line, currentPos);
    SkipWhitespace(line, currentPos);

    m_in_define = true;
    m_current_define = Define(name, std::string());
    m_current_define_value.str(std::string());

    ContinueDefine(line, currentPos);

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

std::unique_ptr<ISimpleExpression> DefinesStreamProxy::ParseExpression(std::shared_ptr<std::string> fileName, int lineNumber, std::string expressionString)
{
    ParserLine pseudoLine(std::move(fileName), lineNumber, std::move(expressionString));
    ExpandDefinedExpressions(pseudoLine);
    ExpandDefines(pseudoLine);

    std::istringstream ss(pseudoLine.m_line);
    ParserSingleInputStream inputStream(ss, "defines directive expression");

    const SimpleExpressionInterpreter expressionInterpreter(&inputStream);
    return expressionInterpreter.Evaluate();
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

    const auto expression = ParseExpression(line.m_filename, line.m_line_number, expressionString);
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

    if (m_modes.top() == BlockMode::IN_BLOCK)
    {
        m_modes.top() = BlockMode::BLOCK_BLOCKED;
        return true;
    }

    if (!SkipWhitespace(line, currentPos))
        throw ParsingException(CreatePos(line, currentPos), "Cannot elif without an expression.");

    const auto expressionString = line.m_line.substr(currentPos, line.m_line.size() - currentPos);

    if (expressionString.empty())
        throw ParsingException(CreatePos(line, currentPos), "Cannot elif without an expression.");

    const auto expression = ParseExpression(line.m_filename, line.m_line_number, expressionString);
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

bool DefinesStreamProxy::MatchDirectives(ParserLine& line)
{
    unsigned directiveStartPos;
    unsigned directiveEndPos;

    if (!FindDirective(line, directiveStartPos, directiveEndPos))
        return false;

    directiveStartPos++;

    if (m_modes.empty() || m_modes.top() == BlockMode::IN_BLOCK)
    {
        if (MatchDefineDirective(line, directiveStartPos, directiveEndPos) || MatchUndefDirective(line, directiveStartPos, directiveEndPos))
            return true;
    }

    return MatchIfdefDirective(line, directiveStartPos, directiveEndPos) || MatchIfDirective(line, directiveStartPos, directiveEndPos)
           || MatchElIfDirective(line, directiveStartPos, directiveEndPos) || MatchElseDirective(line, directiveStartPos, directiveEndPos)
           || MatchEndifDirective(line, directiveStartPos, directiveEndPos);
}

bool DefinesStreamProxy::FindDefineForWord(const std::string& line, const unsigned wordStart, const unsigned wordEnd, const Define*& value) const
{
    const std::string word(line, wordStart, wordEnd - wordStart);
    const auto foundEntry = m_defines.find(word);
    if (foundEntry != m_defines.end())
    {
        value = &foundEntry->second;
        return true;
    }

    return false;
}

void DefinesStreamProxy::ContinueMacroParameters(const ParserLine& line, unsigned& pos)
{
    const auto lineLength = line.m_line.size();
    while (m_macro_parameter_state != ParameterState::NOT_IN_PARAMETERS && pos < lineLength)
    {
        const auto c = line.m_line[pos];

        if (c == ',')
        {
            if (!m_macro_bracket_depth.empty())
            {
                m_macro_parameter_state = ParameterState::AFTER_PARAM;
                m_current_macro_parameter << c;
            }
            else
            {
                m_macro_parameters.emplace_back(m_current_macro_parameter.str());
                m_current_macro_parameter.clear();
                m_current_macro_parameter.str(std::string());
                m_macro_parameter_state = ParameterState::AFTER_COMMA;
            }
        }
        else if (c == '(' || c == '[' || c == '{')
        {
            m_macro_parameter_state = ParameterState::AFTER_PARAM;
            m_macro_bracket_depth.push(c);
            m_current_macro_parameter << c;
        }
        else if (c == ')')
        {
            if (!m_macro_bracket_depth.empty())
            {
                if (m_macro_bracket_depth.top() != '(')
                    throw ParsingException(CreatePos(line, pos), "Unbalanced brackets in macro parameters");

                m_macro_bracket_depth.pop();
                m_macro_parameter_state = ParameterState::AFTER_PARAM;
                m_current_macro_parameter << c;
            }
            else if (m_macro_parameter_state == ParameterState::AFTER_COMMA)
            {
                throw ParsingException(CreatePos(line, pos), "Cannot close macro parameters after comma");
            }
            else
            {
                m_macro_parameters.emplace_back(m_current_macro_parameter.str());
                m_macro_parameter_state = ParameterState::NOT_IN_PARAMETERS;
            }
        }
        else if (c == ']' || c == '}')
        {
            if (!m_macro_bracket_depth.empty())
            {
                const auto otherBracket = c == ']' ? '[' : '{';
                if (m_macro_bracket_depth.top() != otherBracket)
                    throw ParsingException(CreatePos(line, pos), "Unbalanced brackets in macro parameters");
                m_macro_bracket_depth.pop();
            }

            m_macro_parameter_state = ParameterState::AFTER_PARAM;
            m_current_macro_parameter << c;
        }
        else if (m_macro_parameter_state == ParameterState::AFTER_PARAM || !isspace(c))
        {
            m_macro_parameter_state = ParameterState::AFTER_PARAM;
            m_current_macro_parameter << c;
        }

        pos++;
    }
}

void DefinesStreamProxy::ContinueMacro(ParserLine& line)
{
    auto pos = 0u;
    ContinueMacroParameters(line, pos);

    if (m_macro_parameter_state == ParameterState::NOT_IN_PARAMETERS)
    {
        const auto defineValue = m_current_macro->Render(m_macro_parameters);

        if (pos < line.m_line.size())
        {
            std::ostringstream ss;
            ss << defineValue;
            ss << std::string(line.m_line, pos, line.m_line.size() - pos);
            line.m_line = ss.str();
        }
        else
        {
            line.m_line = defineValue;
        }

        ExpandDefines(line);
    }
    else
    {
        line.m_line = "";
    }
}

void DefinesStreamProxy::ExtractParametersFromDefineUsage(const ParserLine& line, const unsigned parameterStart, unsigned& parameterEnd)
{
    if (line.m_line[parameterStart] != '(')
        return;

    m_macro_parameter_state = ParameterState::AFTER_OPEN;
    m_macro_parameters = std::vector<std::string>();
    m_current_macro_parameter.clear();
    m_current_macro_parameter.str(std::string());
    m_macro_bracket_depth = std::stack<char>();
    parameterEnd = parameterStart + 1;

    ContinueMacroParameters(line, parameterEnd);
}

bool DefinesStreamProxy::MatchDefinedExpression(const ParserLine& line, unsigned& pos, std::string& definitionName)
{
    unsigned currentPos = pos;

    if (!MatchNextCharacter(line, currentPos, '('))
        return false;

    const auto nameStartPos = currentPos;
    if (!ExtractIdentifier(line, currentPos))
        return false;

    const auto nameEndPos = currentPos;
    if (!MatchNextCharacter(line, currentPos, ')'))
        return false;

    pos = currentPos;
    definitionName = line.m_line.substr(nameStartPos, nameEndPos - nameStartPos);
    return true;
}

void DefinesStreamProxy::ExpandDefinedExpressions(ParserLine& line) const
{
    auto currentPos = 0u;

    while (true)
    {
        const auto definedPos = line.m_line.find(DEFINED_KEYWORD, currentPos);
        if (definedPos == std::string::npos)
            break;

        currentPos = definedPos;

        if (definedPos > 0 && !isspace(line.m_line[definedPos - 1]))
        {
            currentPos += std::char_traits<char>::length(DEFINED_KEYWORD);
            continue;
        }

        auto definitionEndPos = currentPos + std::char_traits<char>::length(DEFINED_KEYWORD);
        std::string definitionName;
        if (!MatchDefinedExpression(line, definitionEndPos, definitionName))
        {
            currentPos += std::char_traits<char>::length(DEFINED_KEYWORD);
            continue;
        }

        const auto entry = m_defines.find(definitionName);
        line.m_line.replace(currentPos, definitionEndPos - currentPos, entry != m_defines.end() ? "1" : "0");
    }
}

void DefinesStreamProxy::ProcessDefine(const ParserLine& line, unsigned& pos, std::ostringstream& out)
{
    ExtractParametersFromDefineUsage(line, pos, pos);

    if (m_macro_parameter_state == ParameterState::NOT_IN_PARAMETERS)
    {
        const auto defineValue = m_current_macro->Render(m_macro_parameters);
        out << defineValue;
    }
}

bool DefinesStreamProxy::FindNextDefine(const std::string& line, unsigned& pos, unsigned& defineStart, const DefinesStreamProxy::Define*& define)
{
    const auto lineSize = line.size();
    auto wordStart = 0u;
    auto lastWordEnd = 0u;
    auto inWord = false;

    for (; pos < lineSize; pos++)
    {
        const auto c = line[pos];
        if (!inWord)
        {
            if (isalpha(c) || c == '_')
            {
                wordStart = pos;
                inWord = true;
            }
        }
        else
        {
            if (!isalnum(c) && c != '_')
            {
                if (FindDefineForWord(line, wordStart, pos, define))
                {
                    defineStart = wordStart;
                    return true;
                }

                inWord = false;
            }
        }
    }

    if (inWord)
    {
        if (FindDefineForWord(line, wordStart, pos, define))
        {
            defineStart = wordStart;
            return true;
        }
    }

    return false;
}

void DefinesStreamProxy::ExpandDefines(ParserLine& line)
{
    auto defineIterations = 0u;
    bool usesDefines;

    do
    {
        if (defineIterations > MAX_DEFINE_ITERATIONS)
        {
            throw ParsingException(CreatePos(line, 1),
                                   "Potential define loop? Exceeded max define iterations of " + std::to_string(MAX_DEFINE_ITERATIONS) + " iterations.");
        }

        usesDefines = false;
        auto pos = 0u;
        auto defineStart = 0u;
        auto lastDefineEnd = 0u;
        std::ostringstream str;

        while (FindNextDefine(line.m_line, pos, defineStart, m_current_macro))
        {
            if (!usesDefines)
            {
                usesDefines = true;
                str << std::string(line.m_line, 0, defineStart);
            }
            else
            {
                str << std::string(line.m_line, lastDefineEnd, defineStart - (lastDefineEnd));
            }

            ProcessDefine(line, pos, str);

            lastDefineEnd = pos;
        }

        if (usesDefines)
        {
            if (lastDefineEnd < line.m_line.size())
                str << std::string(line.m_line, lastDefineEnd, line.m_line.size() - lastDefineEnd);
            line.m_line = str.str();
        }

        defineIterations++;
    } while (usesDefines);
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

    while (true)
    {
        if (m_in_define)
        {
            unsigned currentPos = 0u;

            if (m_parameter_state != ParameterState::NOT_IN_PARAMETERS)
            {
                ContinueParameters(line, currentPos);
                SkipWhitespace(line, currentPos);
            }

            ContinueDefine(line, currentPos);
            if (!m_skip_directive_lines)
            {
                line.m_line = std::string();
                return line;
            }

            line = m_stream->NextLine();
        }
        else if (m_macro_parameter_state != ParameterState::NOT_IN_PARAMETERS)
        {
            ContinueMacro(line);
            return line;
        }
        else if (MatchDirectives(line) || !m_modes.empty() && m_modes.top() != BlockMode::IN_BLOCK)
        {
            if (!m_skip_directive_lines)
            {
                line.m_line = std::string();
                return line;
            }

            line = m_stream->NextLine();
        }
        else
        {
            ExpandDefines(line);
            return line;
        }
    }
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
