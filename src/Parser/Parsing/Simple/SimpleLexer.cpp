#include "SimpleLexer.h"

SimpleLexer::SimpleLexer(IParserLineStream* stream)
    : AbstractLexer(stream),
    m_config{false, true, true},
    m_last_line(1)
{
}

SimpleLexer::SimpleLexer(IParserLineStream* stream, Config config)
    : AbstractLexer(stream),
    m_config(config),
    m_last_line(1)
{
}

SimpleParserValue SimpleLexer::GetNextToken()
{
    auto c = PeekChar();
    const auto nextCharPos = GetNextCharacterPos();
    if (m_config.m_emit_new_line_tokens && nextCharPos.m_line > m_last_line)
    {
        m_last_line++;
        return SimpleParserValue::NewLine(GetPreviousCharacterPos());
    }

    while (isspace(c))
    {
        if (m_config.m_emit_new_line_tokens && c == '\n')
            return SimpleParserValue::NewLine(GetPreviousCharacterPos());

        NextChar();
        c = PeekChar();
    }
    
    const auto pos = GetNextCharacterPos();
    if (m_config.m_emit_new_line_tokens && pos.m_line > m_last_line)
    {
        m_last_line++;
        return SimpleParserValue::NewLine(GetPreviousCharacterPos());
    }

    c = NextChar();

    if (c == EOF)
        return SimpleParserValue::EndOfFile(TokenPos());

    if (m_config.m_read_strings && c == '\"')
        return SimpleParserValue::String(GetPreviousCharacterPos(), new std::string(ReadString()));
    
    if (m_config.m_read_numbers && (isdigit(c) || c == '.' && isdigit(PeekChar())))
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
