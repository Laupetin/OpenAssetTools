#include "DefinesStreamProxy.h"

#include "ParserSingleInputStream.h"
#include "Parsing/ParsingException.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"
#include "Parsing/Simple/Expression/SimpleExpressionMatchers.h"
#include "Parsing/Simple/SimpleExpressionInterpreter.h"
#include "Utils/StringUtils.h"

#include <regex>
#include <sstream>
#include <utility>

namespace
{
    constexpr auto DEFINE_DIRECTIVE = "define";
    constexpr auto UNDEF_DIRECTIVE = "undef";
    constexpr auto IF_DIRECTIVE = "if";
    constexpr auto ELIF_DIRECTIVE = "elif";
    constexpr auto IFDEF_DIRECTIVE = "ifdef";
    constexpr auto IFNDEF_DIRECTIVE = "ifndef";
    constexpr auto ELSE_DIRECTIVE = "else";
    constexpr auto ENDIF_DIRECTIVE = "endif";
    constexpr auto DEFINED_KEYWORD = "defined";

    bool IsStringizeParameterForwardLookup(const std::string& value, const size_t pos)
    {
        return pos + 1 && (isalpha(value[pos + 1]) || value[pos + 1] == '_');
    }

    bool IsTokenPastingOperatorForwardLookup(const std::string& value, const size_t pos)
    {
        return pos + 1 < value.size() && value[pos + 1] == '#';
    }
} // namespace

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

DefinesStreamProxy::Define::Define()
    : m_contains_token_pasting_operators(false) {};

DefinesStreamProxy::Define::Define(std::string name, std::string value)
    : Define(std::move(name), std::move(value), false)
{
}

DefinesStreamProxy::Define::Define(std::string name, std::string value, const bool parameterized)
    : m_name(std::move(name)),
      m_value(std::move(value)),
      m_contains_token_pasting_operators(false),
      m_parameterized(parameterized)
{
}

DefinesStreamProxy::MacroParameterState::MacroParameterState()
    : m_parameter_state(ParameterState::NOT_IN_PARAMETERS)
{
}

void DefinesStreamProxy::Define::IdentifyTokenPasteOperatorOnly()
{
    for (auto i = 0u; i < m_value.size(); i++)
    {
        if (m_value[i] == '#' && IsTokenPastingOperatorForwardLookup(m_value, i))
        {
            m_contains_token_pasting_operators = true;
            return;
        }
    }
}

void DefinesStreamProxy::Define::IdentifyParameters(const std::vector<std::string>& parameterNames)
{
    if (parameterNames.empty())
    {
        IdentifyTokenPasteOperatorOnly();
        return;
    }

    auto inWord = false;
    auto stringizeNext = false;
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
                    const auto stringizeOffset = stringizeNext ? 1 : 0;

                    m_value.erase(wordStart - stringizeOffset, i - wordStart + stringizeOffset);
                    m_parameter_positions.emplace_back(parameterIndex, wordStart - stringizeOffset, stringizeNext);
                    i = wordStart - stringizeOffset;
                }

                inWord = false;
                stringizeNext = false;
            }

            if (c == '#')
            {
                if (IsStringizeParameterForwardLookup(m_value, i))
                    stringizeNext = true;
                else if (IsTokenPastingOperatorForwardLookup(m_value, i))
                {
                    m_contains_token_pasting_operators = true;

                    // Skip next char since it's # anyway, and we do not want to count it as stringize
                    i++;
                }
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
            const auto stringizeOffset = stringizeNext ? 1 : 0;

            m_value.erase(wordStart - stringizeOffset, m_value.size() - wordStart + stringizeOffset);
            m_parameter_positions.emplace_back(parameterIndex, wordStart - stringizeOffset, stringizeNext);
        }
    }
}

DefinesStreamProxy::DefinesStreamProxy(IParserLineStream* stream, const bool skipDirectiveLines)
    : m_stream(stream),
      m_skip_directive_lines(skipDirectiveLines),
      m_ignore_depth(0),
      m_in_define(false),
      m_parameter_state(ParameterState::NOT_IN_PARAMETERS),
      m_current_macro(nullptr)
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

void DefinesStreamProxy::ContinueDefine(const ParserLine& line, const size_t currentPos)
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

void DefinesStreamProxy::ContinueParameters(const ParserLine& line, size_t& currentPos)
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

void DefinesStreamProxy::MatchDefineParameters(const ParserLine& line, size_t& currentPos)
{
    m_current_define_parameters = std::vector<std::string>();
    m_current_define_parameterized = false;
    if (line.m_line[currentPos] != '(')
        return;

    m_current_define_parameterized = true;
    m_parameter_state = ParameterState::AFTER_OPEN;
    currentPos++;

    ContinueParameters(line, currentPos);
}

bool DefinesStreamProxy::MatchDefineDirective(const ParserLine& line, const size_t directiveStartPosition, const size_t directiveEndPosition)
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
    m_current_define = Define(name, std::string(), m_current_define_parameterized);
    m_current_define_value.str(std::string());

    ContinueDefine(line, currentPos);

    return true;
}

bool DefinesStreamProxy::MatchUndefDirective(const ParserLine& line, const size_t directiveStartPosition, const size_t directiveEndPosition)
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
    ProcessMacrosMultiLine(pseudoLine);

    std::istringstream ss(pseudoLine.m_line);
    ParserSingleInputStream inputStream(ss, "defines directive expression");

    const SimpleExpressionInterpreter expressionInterpreter(&inputStream);
    return expressionInterpreter.Evaluate();
}

bool DefinesStreamProxy::MatchIfDirective(const ParserLine& line, const size_t directiveStartPosition, const size_t directiveEndPosition)
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

bool DefinesStreamProxy::MatchElIfDirective(const ParserLine& line, const size_t directiveStartPosition, const size_t directiveEndPosition)
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

bool DefinesStreamProxy::MatchIfdefDirective(const ParserLine& line, const size_t directiveStartPosition, const size_t directiveEndPosition)
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

bool DefinesStreamProxy::MatchElseDirective(const ParserLine& line, const size_t directiveStartPosition, const size_t directiveEndPosition)
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

bool DefinesStreamProxy::MatchEndifDirective(const ParserLine& line, const size_t directiveStartPosition, const size_t directiveEndPosition)
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
    size_t directiveStartPos;
    size_t directiveEndPos;

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

bool DefinesStreamProxy::FindMacroForIdentifier(const std::string& input,
                                                const unsigned identifierStart,
                                                const unsigned identifierEnd,
                                                const Define*& value) const
{
    const std::string identifier(input, identifierStart, identifierEnd - identifierStart);
    const auto foundEntry = m_defines.find(identifier);
    if (foundEntry != m_defines.end())
    {
        value = &foundEntry->second;
        return true;
    }

    return false;
}

void DefinesStreamProxy::ExtractParametersFromMacroUsage(
    const ParserLine& line, const unsigned& linePos, MacroParameterState& state, const std::string& input, unsigned& inputPos)
{
    if (input[inputPos] != '(')
        return;

    inputPos++;
    state.m_parameter_state = ParameterState::AFTER_OPEN;
    state.m_parameters = std::vector<std::string>();
    state.m_current_parameter.clear();
    state.m_current_parameter.str(std::string());
    state.m_bracket_depth = std::stack<char>();

    ContinueMacroParameters(line, linePos, state, input, inputPos);
}

bool DefinesStreamProxy::MatchDefinedExpression(const ParserLine& line, size_t& pos, std::string& definitionName)
{
    auto currentPos = pos;

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
    auto currentPos = 0uz;

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

bool DefinesStreamProxy::FindNextMacro(const std::string& input, unsigned& inputPos, unsigned& defineStart, const Define*& define) const
{
    const auto inputSize = input.size();
    auto wordStart = 0u;
    auto inWord = false;
    auto inString = false;
    auto stringEscape = false;

    for (; inputPos < inputSize; inputPos++)
    {
        const auto c = input[inputPos];
        if (inString)
        {
            if (!stringEscape)
            {
                if (c == '"')
                    inString = false;
                else if (c == '\\')
                    stringEscape = true;
            }
            else
                stringEscape = false;
        }
        else
        {
            if (c == '"')
                inString = true;

            if (!inWord)
            {
                if (isalpha(c) || c == '_')
                {
                    wordStart = inputPos;
                    inWord = true;
                }
            }
            else
            {
                if (!isalnum(c) && c != '_')
                {
                    if (FindMacroForIdentifier(input, wordStart, inputPos, define))
                    {
                        defineStart = wordStart;
                        return true;
                    }

                    inWord = false;
                }
            }
        }
    }

    if (inWord)
    {
        if (FindMacroForIdentifier(input, wordStart, inputPos, define))
        {
            defineStart = wordStart;
            return true;
        }
    }

    return false;
}

namespace
{
    enum class TokenPasteTokenType : std::uint8_t
    {
        NONE,
        STRING,
        IDENTIFIER,
        SYMBOL
    };

    class TokenPasteToken
    {
    public:
        TokenPasteToken()
            : m_type(TokenPasteTokenType::NONE),
              m_start(0u),
              m_end(0u)
        {
        }

        void SetFromInput(const ParserLine& line, const unsigned& linePos, const std::string& input, unsigned& offset)
        {
            m_start = offset;

            const auto firstChar = input[offset++];
            const auto inputSize = input.size();
            if (firstChar == '"')
            {
                m_type = TokenPasteTokenType::STRING;
                for (; offset < inputSize; offset++)
                {
                    const auto c = input[offset];
                    if (c == '\\')
                        offset++; // Skip next char
                    else if (c == '"')
                        break;
                }

                if (offset >= inputSize)
                    throw ParsingException(TokenPos(*line.m_filename, line.m_line_number, static_cast<int>(linePos + 1)),
                                           "Token-pasting operator cannot be used on unclosed string");

                offset++;
            }
            else if (isalpha(firstChar) || firstChar == '_')
            {
                m_type = TokenPasteTokenType::IDENTIFIER;
                for (; offset < inputSize; offset++)
                {
                    const auto c = input[offset];
                    if (!isalnum(c) && c != '_')
                        break;
                }
            }
            else
            {
                m_type = TokenPasteTokenType::SYMBOL;
            }

            m_end = offset;
        }

        void EmitValue(std::ostream& out, const std::string& input) const
        {
            if (m_end <= m_start)
                return;

            if (m_type == TokenPasteTokenType::STRING)
            {
                if (m_end - m_start > 2)
                    out << std::string(input, m_start + 1, m_end - m_start - 2);
            }
            else
            {
                assert(m_type == TokenPasteTokenType::IDENTIFIER || m_type == TokenPasteTokenType::SYMBOL);
                out << std::string(input, m_start, m_end - m_start);
            }
        }

        TokenPasteTokenType m_type;
        unsigned m_start;
        unsigned m_end;
    };

    void EmitPastedTokens(const ParserLine& line,
                          const unsigned& linePos,
                          std::ostream& out,
                          const std::string& input,
                          const TokenPasteToken& token0,
                          const TokenPasteToken& token1)
    {
        if ((token0.m_type == TokenPasteTokenType::STRING) != (token1.m_type == TokenPasteTokenType::STRING))
            throw ParsingException(TokenPos(*line.m_filename, line.m_line_number, static_cast<int>(linePos + 1)),
                                   "String token can only use token-pasting operator on other string token");
        if (token0.m_type == TokenPasteTokenType::STRING)
        {
            out << '"';
            token0.EmitValue(out, input);
            token1.EmitValue(out, input);
            out << '"';
        }
        else
        {
            assert(token0.m_type == TokenPasteTokenType::IDENTIFIER || token0.m_type == TokenPasteTokenType::SYMBOL);

            token0.EmitValue(out, input);
            token1.EmitValue(out, input);
        }
    }
} // namespace

void DefinesStreamProxy::ProcessTokenPastingOperators(const ParserLine& line, const unsigned& linePos, std::string& input)
{
    std::ostringstream ss;

    auto pasteNext = false;
    TokenPasteToken previousToken;
    TokenPasteToken currentToken;

    const auto inputSize = input.size();
    for (auto i = 0u; i < inputSize;)
    {
        const auto c = input[i];

        if (isspace(c))
        {
            i++;
            continue;
        }

        if (c == '#' && IsTokenPastingOperatorForwardLookup(input, i))
        {
            if (currentToken.m_type == TokenPasteTokenType::NONE)
                throw ParsingException(CreatePos(line, linePos), "Cannot use token-pasting operator without previous token");

            if (previousToken.m_end < currentToken.m_start)
                ss << std::string(input, previousToken.m_end, currentToken.m_start - previousToken.m_end);

            previousToken = currentToken;
            pasteNext = true;

            // Skip second #
            i += 2;
        }
        else
        {
            currentToken.SetFromInput(line, linePos, input, i);
            if (pasteNext)
            {
                EmitPastedTokens(line, linePos, ss, input, previousToken, currentToken);
                previousToken = currentToken;
                pasteNext = false;
            }
        }
    }

    if (inputSize > previousToken.m_end)
        ss << std::string(input, previousToken.m_end, inputSize - previousToken.m_end);

    if (pasteNext)
        throw ParsingException(CreatePos(line, linePos), "Cannot use token-pasting operator without following token");

    input = ss.str();
}

void DefinesStreamProxy::InsertMacroParameters(std::ostringstream& out, const Define* macro, const std::vector<std::string>& parameterValues)
{
    if (parameterValues.empty() || macro->m_parameter_positions.empty())
    {
        out << macro->m_value;
        return;
    }

    auto lastPos = 0uz;
    for (const auto& parameterPosition : macro->m_parameter_positions)
    {
        if (lastPos < parameterPosition.m_parameter_position)
            out << std::string(macro->m_value, lastPos, parameterPosition.m_parameter_position - lastPos);

        if (parameterPosition.m_parameter_index < parameterValues.size())
        {
            if (!parameterPosition.m_stringize)
            {
                out << parameterValues[parameterPosition.m_parameter_index];
            }
            else
                out << '"' << utils::EscapeStringForQuotationMarks(parameterValues[parameterPosition.m_parameter_index]) << '"';
        }

        lastPos = parameterPosition.m_parameter_position;
    }

    if (lastPos < macro->m_value.size())
        out << std::string(macro->m_value, lastPos, macro->m_value.size() - lastPos);
}

void DefinesStreamProxy::ExpandMacro(ParserLine& line,
                                     unsigned& linePos,
                                     std::ostringstream& out,
                                     std::vector<const Define*>& callstack,
                                     const Define* macro,
                                     const std::vector<std::string>& parameterValues)
{
    std::ostringstream rawOutput;
    InsertMacroParameters(rawOutput, macro, parameterValues);

    std::string str = rawOutput.str();
    ProcessNestedMacros(line, linePos, callstack, str);

    if (macro->m_contains_token_pasting_operators)
        ProcessTokenPastingOperators(line, linePos, str);

    out << str;
}

void DefinesStreamProxy::ContinueMacroParameters(
    const ParserLine& line, const unsigned& linePos, MacroParameterState& state, const std::string& input, unsigned& inputPos)
{
    const auto inputLength = input.size();
    while (state.m_parameter_state != ParameterState::NOT_IN_PARAMETERS && inputPos < inputLength)
    {
        const auto c = input[inputPos];

        if (c == ',')
        {
            if (!state.m_bracket_depth.empty())
            {
                state.m_parameter_state = ParameterState::AFTER_PARAM;
                state.m_current_parameter << c;
            }
            else
            {
                state.m_parameters.emplace_back(state.m_current_parameter.str());
                state.m_current_parameter.clear();
                state.m_current_parameter.str(std::string());
                state.m_parameter_state = ParameterState::AFTER_COMMA;
            }
        }
        else if (c == '(' || c == '[' || c == '{')
        {
            state.m_parameter_state = ParameterState::AFTER_PARAM;
            state.m_bracket_depth.push(c);
            state.m_current_parameter << c;
        }
        else if (c == ')')
        {
            if (!state.m_bracket_depth.empty())
            {
                if (state.m_bracket_depth.top() != '(')
                    throw ParsingException(CreatePos(line, linePos), "Unbalanced brackets in macro parameters");

                state.m_bracket_depth.pop();
                state.m_parameter_state = ParameterState::AFTER_PARAM;
                state.m_current_parameter << c;
            }
            else if (state.m_parameter_state == ParameterState::AFTER_COMMA)
            {
                throw ParsingException(CreatePos(line, linePos), "Cannot close macro parameters after comma");
            }
            else
            {
                state.m_parameters.emplace_back(state.m_current_parameter.str());
                state.m_parameter_state = ParameterState::NOT_IN_PARAMETERS;
            }
        }
        else if (c == ']' || c == '}')
        {
            if (!state.m_bracket_depth.empty())
            {
                const auto otherBracket = c == ']' ? '[' : '{';
                if (state.m_bracket_depth.top() != otherBracket)
                    throw ParsingException(CreatePos(line, linePos), "Unbalanced brackets in macro parameters");
                state.m_bracket_depth.pop();
            }

            state.m_parameter_state = ParameterState::AFTER_PARAM;
            state.m_current_parameter << c;
        }
        else if (state.m_parameter_state == ParameterState::AFTER_PARAM || !isspace(c))
        {
            state.m_parameter_state = ParameterState::AFTER_PARAM;
            state.m_current_parameter << c;
        }

        inputPos++;
    }
}

void DefinesStreamProxy::ContinueMultiLineMacro(ParserLine& line)
{
    auto pos = 0u;
    ContinueMacroParameters(line, pos, m_multi_line_macro_parameters, line.m_line, pos);

    if (m_multi_line_macro_parameters.m_parameter_state == ParameterState::NOT_IN_PARAMETERS)
    {
        std::ostringstream ss;
        std::vector<const Define*> callstack;
        ExpandMacro(line, pos, ss, callstack, m_current_macro, m_multi_line_macro_parameters.m_parameters);

        if (pos < line.m_line.size())
            ss << std::string(line.m_line, pos, line.m_line.size() - pos);

        line.m_line = ss.str();

        ProcessMacrosMultiLine(line);
    }
    else
    {
        line.m_line = std::string();
    }
}

void DefinesStreamProxy::ProcessNestedMacros(ParserLine& line, unsigned& linePos, std::vector<const Define*>& callstack, std::string& input)
{
    bool usesDefines = false;

    auto pos = 0u;
    auto defineStart = 0u;
    auto lastDefineEnd = 0u;
    std::ostringstream ss;

    const Define* nestedMacro = nullptr;
    while (FindNextMacro(input, pos, defineStart, nestedMacro))
    {
        if (std::ranges::find(std::as_const(callstack), nestedMacro) != callstack.cend())
        {
            // Do not expand recursively
            continue;
        }

        // Make sure we account for all text between the last macro (or beginning) and now
        if (!usesDefines)
        {
            usesDefines = true;
            ss << std::string(input, 0, defineStart);
        }
        else
        {
            ss << std::string(input, lastDefineEnd, defineStart - (lastDefineEnd));
        }

        callstack.emplace_back(nestedMacro);

        MacroParameterState nestedMacroState;
        if (nestedMacro->m_parameterized)
        {
            ExtractParametersFromMacroUsage(line, linePos, nestedMacroState, input, pos);
            if (nestedMacroState.m_parameter_state != ParameterState::NOT_IN_PARAMETERS)
                throw ParsingException(CreatePos(line, linePos), "Unbalanced brackets in macro parameters");
        }
        ExpandMacro(line, linePos, ss, callstack, nestedMacro, nestedMacroState.m_parameters);

        callstack.pop_back();

        lastDefineEnd = pos;
    }

    if (usesDefines)
    {
        // Make sure we account for all text between the last macro and the end
        if (lastDefineEnd < input.size())
            ss << std::string(input, lastDefineEnd, input.size() - lastDefineEnd);
        input = ss.str();
    }
}

void DefinesStreamProxy::ProcessMacrosSingleLine(ParserLine& line)
{
    unsigned pos = 0;
    std::vector<const Define*> callstack;
    ProcessNestedMacros(line, pos, callstack, line.m_line);
}

void DefinesStreamProxy::ProcessMacrosMultiLine(ParserLine& line)
{
    bool usesDefines = false;

    auto pos = 0u;
    auto defineStart = 0u;
    auto lastDefineEnd = 0u;
    std::ostringstream str;
    std::vector<const Define*> callstack;
    while (FindNextMacro(line.m_line, pos, defineStart, m_current_macro))
    {
        // Make sure we account for all text between the last macro (or beginning) and now
        if (!usesDefines)
        {
            usesDefines = true;
            str << std::string(line.m_line, 0, defineStart);
        }
        else
        {
            str << std::string(line.m_line, lastDefineEnd, defineStart - (lastDefineEnd));
        }

        callstack.emplace_back(m_current_macro);

        if (m_current_macro->m_parameterized)
            ExtractParametersFromMacroUsage(line, pos, m_multi_line_macro_parameters, line.m_line, pos);

        // If still in parameters they continue on the next line
        if (m_multi_line_macro_parameters.m_parameter_state == ParameterState::NOT_IN_PARAMETERS)
            ExpandMacro(line, pos, str, callstack, m_current_macro, m_multi_line_macro_parameters.m_parameters);

        callstack.pop_back();

        lastDefineEnd = pos;
    }

    if (usesDefines)
    {
        // Make sure we account for all text between the last macro and the end
        if (lastDefineEnd < line.m_line.size())
            str << std::string(line.m_line, lastDefineEnd, line.m_line.size() - lastDefineEnd);
        line.m_line = str.str();
    }
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
            auto currentPos = 0uz;

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
        else if (m_multi_line_macro_parameters.m_parameter_state != ParameterState::NOT_IN_PARAMETERS)
        {
            ContinueMultiLineMacro(line);
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
            ProcessMacrosMultiLine(line);
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
