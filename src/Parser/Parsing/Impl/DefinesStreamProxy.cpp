#include "DefinesStreamProxy.h"

#include <sstream>
#include <utility>

#include "Parsing/ParsingException.h"

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
        if (!isalnum(c))
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
      m_ignore_depth(0)
{
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

bool DefinesStreamProxy::MatchDefineDirective(const ParserLine& line, unsigned directivePosition)
{
    if (!MatchString(line, directivePosition, DEFINE_DIRECTIVE, std::char_traits<char>::length(DEFINE_DIRECTIVE)))
        return false;

    const auto nameStartPos = directivePosition;
    if (!ExtractIdentifier(line, directivePosition))
        throw ParsingException(CreatePos(line, directivePosition), "Cannot ifdef without a name.");

    const auto name = line.m_line.substr(nameStartPos, directivePosition - nameStartPos);

    const auto parameters = MatchDefineParameters(line, directivePosition);

    std::string value;
    if (directivePosition < line.m_line.size())
        value = line.m_line.substr(directivePosition + 1);

    Define define(name, value);
    define.IdentifyParameters(parameters);
    AddDefine(std::move(define));

    return true;
}

bool DefinesStreamProxy::MatchUndefDirective(const ParserLine& line, unsigned directivePosition)
{
    if (!MatchString(line, directivePosition, UNDEF_DIRECTIVE, std::char_traits<char>::length(UNDEF_DIRECTIVE)))
        return false;

    if (!SkipWhitespace(line, directivePosition))
        throw ParsingException(CreatePos(line, directivePosition), "Cannot undef without a name.");

    const auto nameStartPos = directivePosition;
    if (!ExtractIdentifier(line, directivePosition))
        throw ParsingException(CreatePos(line, directivePosition), "Cannot ifdef without a name.");

    const auto name = line.m_line.substr(nameStartPos, directivePosition - nameStartPos);
    const auto entry = m_defines.find(name);

    if (entry != m_defines.end())
        m_defines.erase(entry);

    return true;
}

bool DefinesStreamProxy::MatchIfdefDirective(const ParserLine& line, unsigned directivePosition)
{
    auto reverse = false;
    if (!MatchString(line, directivePosition, IFDEF_DIRECTIVE, std::char_traits<char>::length(IFDEF_DIRECTIVE)))
    {
        if (!MatchString(line, directivePosition, IFNDEF_DIRECTIVE, std::char_traits<char>::length(IFNDEF_DIRECTIVE)))
            return false;

        reverse = true;
    }

    if (!m_modes.empty() && !m_modes.top())
    {
        m_ignore_depth++;
        return true;
    }

    if (!SkipWhitespace(line, directivePosition))
        throw ParsingException(CreatePos(line, directivePosition), "Cannot ifdef without a name.");

    const auto nameStartPos = directivePosition;
    if (!ExtractIdentifier(line, directivePosition))
        throw ParsingException(CreatePos(line, directivePosition), "Cannot ifdef without a name.");

    const auto name = line.m_line.substr(nameStartPos, directivePosition - nameStartPos);
    const auto entry = m_defines.find(name);

    if (entry != m_defines.end())
        m_modes.push(!reverse);
    else
        m_modes.push(reverse);

    return true;
}

bool DefinesStreamProxy::MatchElseDirective(const ParserLine& line, unsigned directivePosition)
{
    if (!MatchString(line, directivePosition, ELSE_DIRECTIVE, std::char_traits<char>::length(ELSE_DIRECTIVE)))
        return false;

    if (m_ignore_depth > 0)
        return true;

    if (!m_modes.empty())
        m_modes.top() = !m_modes.top();
    else
        throw ParsingException(CreatePos(line, directivePosition), "Cannot use else without ifdef");

    return true;
}

bool DefinesStreamProxy::MatchEndifDirective(const ParserLine& line, unsigned directivePosition)
{
    if (!MatchString(line, directivePosition, ENDIF_DIRECTIVE, std::char_traits<char>::length(ENDIF_DIRECTIVE)))
        return false;

    if (m_ignore_depth > 0)
    {
        m_ignore_depth--;
        return true;
    }

    if (!m_modes.empty())
        m_modes.pop();
    else
        throw ParsingException(CreatePos(line, directivePosition), "Cannot use endif without ifdef");

    return true;
}

bool DefinesStreamProxy::MatchDirectives(const ParserLine& line)
{
    unsigned directivePos;

    if (!FindDirective(line, directivePos))
        return false;

    directivePos++;

    if (m_modes.empty() || m_modes.top() == true)
    {
        if (MatchDefineDirective(line, directivePos)
            || MatchUndefDirective(line, directivePos))
            return true;
    }

    return MatchIfdefDirective(line, directivePos)
        || MatchElseDirective(line, directivePos)
        || MatchEndifDirective(line, directivePos);
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
    while (true)
    {
        if (pos >= line.m_line.size())
            throw ParsingException(CreatePos(line, pos), "Invalid use of define");

        const auto c = line.m_line[pos];

        if (c == ',')
        {
            parameterValues.emplace_back(currentValue.str());
            currentValue.clear();
            currentValue.str(std::string());
            valueHasStarted = false;
        }
        else if (c == ')')
        {
            parameterValues.emplace_back(currentValue.str());
            parameterEnd = pos + 1;
            break;
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

    if (MatchDirectives(line)
        || !m_modes.empty() && !m_modes.top())
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
