#include "ZoneDefinitionLexer.h"

#include <sstream>

ZoneDefinitionLexer::ZoneDefinitionLexer(IParserLineStream* stream)
    : AbstractLexer(stream)
{
}

std::string ZoneDefinitionLexer::ReadField()
{
    const auto& currentLine = CurrentLine();
    assert(m_current_line_offset >= 1);

    const auto startPos = m_current_line_offset - 1;
    const auto lineSize = currentLine.m_line.size();
    while (m_current_line_offset < lineSize)
    {
        const auto c = currentLine.m_line[m_current_line_offset];

        bool isNonFieldCharacter;
        switch (c)
        {
        case '\"':
        case '>':
        case '<':
        case ',':
            isNonFieldCharacter = true;
            break;
        default:
            isNonFieldCharacter = false;
            break;
        }

        if (isNonFieldCharacter)
            break;

        m_current_line_offset++;
    }

    return std::string(currentLine.m_line, startPos, m_current_line_offset - startPos);
}

ZoneDefinitionParserValue ZoneDefinitionLexer::GetNextToken()
{
    auto c = NextChar();

    while (c != EOF)
    {
        switch (c)
        {
        case '\"':
            return ZoneDefinitionParserValue::String(GetPreviousCharacterPos(), new std::string(ReadString()));

        case '>':
        case '<':
        case ',':
            return ZoneDefinitionParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));

        default:
            if (isspace(c))
                break;

            return ZoneDefinitionParserValue::Field(GetPreviousCharacterPos(), new std::string(ReadField()));
        }

        c = NextChar();
    }

    return ZoneDefinitionParserValue::EndOfFile(TokenPos());
}
