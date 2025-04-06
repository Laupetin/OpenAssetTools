#include "AbstractDirectiveStreamProxy.h"

TokenPos AbstractDirectiveStreamProxy::CreatePos(const ParserLine& line, const size_t position)
{
    return TokenPos(*line.m_filename, line.m_line_number, static_cast<int>(position + 1));
}

bool AbstractDirectiveStreamProxy::SkipWhitespace(const ParserLine& line, size_t& position)
{
    while (true)
    {
        if (position >= line.m_line.size())
            return false;

        if (isspace(line.m_line[position]))
            position++;
        else
            break;
    }

    return true;
}

bool AbstractDirectiveStreamProxy::ExtractInteger(const ParserLine& line, size_t& position, int& value)
{
    if (position >= line.m_line.size())
        return false;

    const auto* startPosition = &line.m_line[position];
    char* endPosition;
    value = strtol(startPosition, &endPosition, 0);
    const auto len = endPosition - startPosition;

    if (len > 0)
    {
        position += len;
        return true;
    }

    return false;
}

bool AbstractDirectiveStreamProxy::ExtractIdentifier(const ParserLine& line, size_t& position)
{
    auto firstChar = true;
    while (true)
    {
        if (position >= line.m_line.size())
            return !firstChar;

        const auto c = line.m_line[position];
        if (isalpha(c) || c == '_' || !firstChar && isdigit(c))
        {
            position++;
        }
        else
            return !firstChar;

        firstChar = false;
    }
}

bool AbstractDirectiveStreamProxy::MatchCharacter(const ParserLine& line, size_t& position, char c)
{
    if (position < line.m_line.size() && line.m_line[position] == c)
    {
        position++;
        return true;
    }

    return false;
}

bool AbstractDirectiveStreamProxy::MatchNextCharacter(const ParserLine& line, size_t& position, char c)
{
    return SkipWhitespace(line, position) && MatchCharacter(line, position, c);
}

bool AbstractDirectiveStreamProxy::MatchString(const ParserLine& line, size_t& position, const char* str, size_t len)
{
    if (line.m_line.compare(position, len, str) == 0)
    {
        position += len;
        return true;
    }

    return false;
}

bool AbstractDirectiveStreamProxy::MatchNextString(const ParserLine& line, size_t& position, const char* str, size_t len)
{
    return SkipWhitespace(line, position) && MatchString(line, position, str, len);
}

bool AbstractDirectiveStreamProxy::FindDirective(const ParserLine& line, size_t& directiveStartPosition, size_t& directiveEndPos)
{
    directiveStartPosition = 0;
    for (; directiveStartPosition < line.m_line.size(); directiveStartPosition++)
    {
        const auto c = line.m_line[directiveStartPosition];

        if (isspace(c))
            continue;

        if (c != '#')
            continue;

        directiveEndPos = directiveStartPosition + 1;
        for (; directiveEndPos < line.m_line.size(); directiveEndPos++)
        {
            const auto c2 = line.m_line[directiveEndPos];

            if (isspace(c2))
                break;
        }

        return true;
    }

    return false;
}
