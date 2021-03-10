#include "SimpleMatcherAnyCharacterBesides.h"

SimpleMatcherAnyCharacterBesides::SimpleMatcherAnyCharacterBesides(std::vector<char> chars)
    : m_chars(std::move(chars))
{
}

MatcherResult<SimpleParserValue> SimpleMatcherAnyCharacterBesides::CanMatch(ILexer<SimpleParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);
    return token.m_type == SimpleParserValueType::CHARACTER && std::find(m_chars.begin(), m_chars.end(), token.CharacterValue()) == m_chars.end()
               ? MatcherResult<SimpleParserValue>::Match(1)
               : MatcherResult<SimpleParserValue>::NoMatch();
}
