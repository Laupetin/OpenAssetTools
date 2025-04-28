#include "SimpleLexer.h"

SimpleLexer::Config::MultiCharacterToken::MultiCharacterToken(const int id, std::string value)
    : m_id(id),
      m_value(std::move(value))
{
}

SimpleLexer::MultiCharacterTokenLookupEntry::MultiCharacterTokenLookupEntry(const int id, std::string value)
    : m_id(id),
      m_value(std::move(value))
{
}

SimpleLexer::SimpleLexer(IParserLineStream* stream)
    : AbstractLexer(stream),
      m_config{.m_emit_new_line_tokens = false,
               .m_read_strings = true,
               .m_string_escape_sequences = false,
               .m_read_integer_numbers = true,
               .m_read_floating_point_numbers = true,
               .m_multi_character_tokens = {}},
      m_check_for_multi_character_tokens(false),
      m_last_line(1)
{
}

SimpleLexer::SimpleLexer(IParserLineStream* stream, Config config)
    : AbstractLexer(stream),
      m_config(std::move(config)),
      m_check_for_multi_character_tokens(false),
      m_last_line(1)
{
    for (auto tokenConfig : m_config.m_multi_character_tokens)
        AddMultiCharacterTokenConfigToLookup(std::move(tokenConfig));
    m_config.m_multi_character_tokens.clear();

    // If reading floating point numbers then must be reading integers
    assert(m_config.m_read_floating_point_numbers == false || m_config.m_read_floating_point_numbers == m_config.m_read_integer_numbers);
}

void SimpleLexer::AddMultiCharacterTokenConfigToLookup(Config::MultiCharacterToken tokenConfig)
{
    if (tokenConfig.m_value.empty())
        return;

    m_check_for_multi_character_tokens = true;
    const auto firstCharacterValue = static_cast<uint8_t>(tokenConfig.m_value[0]);

    if (m_multi_character_token_lookup[firstCharacterValue])
    {
        auto* currentEntry = m_multi_character_token_lookup[firstCharacterValue].get();
        while (currentEntry->m_next)
            currentEntry = currentEntry->m_next.get();

        currentEntry->m_next = std::make_unique<MultiCharacterTokenLookupEntry>(tokenConfig.m_id, std::move(tokenConfig.m_value));
    }
    else
    {
        m_multi_character_token_lookup[firstCharacterValue] =
            std::make_unique<MultiCharacterTokenLookupEntry>(tokenConfig.m_id, std::move(tokenConfig.m_value));
    }
}

bool SimpleLexer::ReadMultiCharacterToken(const MultiCharacterTokenLookupEntry* multiTokenLookup)
{
    const auto& currentLine = CurrentLine();
    assert(m_current_line_offset >= 1);
    assert(multiTokenLookup);
    assert(!multiTokenLookup->m_value.empty());
    assert(currentLine.m_line[m_current_line_offset - 1] == multiTokenLookup->m_value[0]);

    const char* linePos = &currentLine.m_line[m_current_line_offset - 1];

    for (const auto c : multiTokenLookup->m_value)
    {
        if (!*linePos || *linePos != c)
            return false;
        linePos++;
    }

    m_current_line_offset = m_current_line_offset - 1 + static_cast<unsigned>(multiTokenLookup->m_value.size());
    return true;
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

    if (m_check_for_multi_character_tokens)
    {
        const auto* multiTokenLookup = m_multi_character_token_lookup[static_cast<uint8_t>(c)].get();
        while (multiTokenLookup)
        {
            if (ReadMultiCharacterToken(multiTokenLookup))
                return SimpleParserValue::MultiCharacter(pos, multiTokenLookup->m_id);

            multiTokenLookup = multiTokenLookup->m_next.get();
        }
    }

    if (m_config.m_read_strings && c == '\"')
        return SimpleParserValue::String(pos, new std::string(m_config.m_string_escape_sequences ? ReadStringWithEscapeSequences() : ReadString()));

    if (m_config.m_read_integer_numbers
        && (isdigit(c) || (c == '+' || c == '-' || (m_config.m_read_floating_point_numbers && c == '.')) && isdigit(PeekChar())))
    {
        bool hasSignPrefix;
        int integerValue;

        if (m_config.m_read_floating_point_numbers)
        {
            bool isFloatingPointValue;
            double floatingPointValue;

            ReadNumber(isFloatingPointValue, hasSignPrefix, floatingPointValue, integerValue);

            if (isFloatingPointValue)
                return SimpleParserValue::FloatingPoint(pos, floatingPointValue, hasSignPrefix);

            return SimpleParserValue::Integer(pos, integerValue, hasSignPrefix);
        }

        ReadNumber(hasSignPrefix, integerValue);
        return SimpleParserValue::Integer(pos, integerValue, hasSignPrefix);
    }

    if (isalpha(c) || c == '_')
    {
        auto identifier = ReadIdentifier();

        return SimpleParserValue::Identifier(pos, new std::string(std::move(identifier)));
    }

    return SimpleParserValue::Character(pos, static_cast<char>(c));
}
