#include "SimpleLexer.h"

SimpleLexer::SimpleLexer(IParserLineStream* stream)
    : AbstractLexer(stream),
      m_emit_new_line_tokens(false),
      m_read_strings(true),
      m_read_numbers(true),
      m_last_line(1)
{
}

void SimpleLexer::SetShouldEmitNewLineTokens(const bool value)
{
    m_emit_new_line_tokens = value;
}

void SimpleLexer::SetShouldReadStrings(const bool value)
{
    m_read_strings = value;
}

void SimpleLexer::SetShouldReadNumbers(const bool value)
{
    m_read_numbers = value;
}

SimpleParserValue SimpleLexer::GetNextToken()
{
    auto c = PeekChar();
    const auto nextCharPos = GetNextCharacterPos();
    if (m_emit_new_line_tokens && nextCharPos.m_line > m_last_line)
    {
        m_last_line++;
        return SimpleParserValue::NewLine(GetPreviousCharacterPos());
    }

    while (isspace(c))
    {
        if (m_emit_new_line_tokens && c == '\n')
            return SimpleParserValue::NewLine(GetPreviousCharacterPos());

        NextChar();
        c = PeekChar();
    }
    
    const auto pos = GetNextCharacterPos();
    if (m_emit_new_line_tokens && pos.m_line > m_last_line)
    {
        m_last_line++;
        return SimpleParserValue::NewLine(GetPreviousCharacterPos());
    }

    c = NextChar();

    if (c == EOF)
        return SimpleParserValue::EndOfFile(TokenPos());

    if (m_read_strings && c == '\"')
        return SimpleParserValue::String(GetPreviousCharacterPos(), new std::string(ReadString()));
    
    if (m_read_numbers && isdigit(c))
    {
        bool isFloatingPointValue;
        double doubleValue;
        int integerValue;

        ReadNumber(isFloatingPointValue, doubleValue, integerValue);

        if (isFloatingPointValue)
            return SimpleParserValue::FloatingPoint(pos, doubleValue);

        return SimpleParserValue::Integer(pos, integerValue);
    }

    if (isalpha(c) || c == '_')
    {
        auto identifier = ReadIdentifier();

        return SimpleParserValue::Identifier(pos, new std::string(std::move(identifier)));
    }

    return SimpleParserValue::Character(pos, static_cast<char>(c));
}
