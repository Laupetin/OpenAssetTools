#include "DefinesStreamProxy.h"

#include <sstream>
#include <utility>

#include "Parsing/ParsingException.h"

DefinesStreamProxy::DefinesStreamProxy(IParserLineStream* stream)
    : m_stream(stream),
      m_ignore_depth(0)
{
}

bool DefinesStreamProxy::MatchDefineDirective(const ParserLine& line, unsigned directivePosition)
{
    if (!MatchString(line, directivePosition, DEFINE_DIRECTIVE, std::char_traits<char>::length(DEFINE_DIRECTIVE)))
        return false;

    const auto nameStartPos = directivePosition;
    if (!ExtractIdentifier(line, directivePosition))
        throw ParsingException(CreatePos(line, directivePosition), "Cannot ifdef without a name.");

    const auto name = line.m_line.substr(nameStartPos, directivePosition - nameStartPos);
    std::string value;
    if (directivePosition < line.m_line.size())
        value = line.m_line.substr(directivePosition + 1);
    m_defines[name] = value;

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
    if(!MatchString(line, directivePosition, IFDEF_DIRECTIVE, std::char_traits<char>::length(IFDEF_DIRECTIVE)))
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
    if(!ExtractIdentifier(line, directivePosition))
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

    if(m_modes.empty() || m_modes.top() == true)
    {
        if (MatchDefineDirective(line, directivePos)
            || MatchUndefDirective(line, directivePos))
            return true;
    }

    return MatchIfdefDirective(line, directivePos)
        || MatchElseDirective(line, directivePos)
        || MatchEndifDirective(line, directivePos);
}

bool DefinesStreamProxy::FindDefineForWord(const ParserLine& line, const unsigned wordStart, const unsigned wordEnd, std::string& value)
{
    const std::string word(line.m_line, wordStart, wordEnd - wordStart);
    const auto foundEntry = m_defines.find(word);
    if(foundEntry != m_defines.end())
    {
        value = foundEntry->second;
        return true;
    }

    return false;
}

void DefinesStreamProxy::ExpandDefines(ParserLine& line)
{
    auto wordStart = 0u;
    auto lastWordEnd = 0u;
    auto inWord = false;
    std::string value;

    std::ostringstream str;
    auto usesDefines = false;

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
                    if (!usesDefines)
                    {
                        str << std::string(line.m_line, 0, wordStart) << value;
                        usesDefines = true;
                    }
                    else
                    {
                        str << std::string(line.m_line, lastWordEnd, wordStart - lastWordEnd) << value;
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
            if (!usesDefines)
            {
                str << std::string(line.m_line, 0, wordStart) << value;
                usesDefines = true;
            }
            else
            {
                str << std::string(line.m_line, lastWordEnd, wordStart - lastWordEnd) << value;
            }
            lastWordEnd = line.m_line.size();
        }
    }

    if(usesDefines)
    {
        if (lastWordEnd < line.m_line.size())
            str << std::string(line.m_line, lastWordEnd, line.m_line.size() - lastWordEnd);
        line.m_line = str.str();
    }
}

void DefinesStreamProxy::AddDefine(const std::string& name, std::string value)
{
    m_defines[name] = std::move(value);
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
