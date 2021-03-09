#include "SimpleMatcherCharacter.h"

SimpleMatcherCharacter::SimpleMatcherCharacter(const char c)
    : m_char(c)
{
}

MatcherResult<SimpleParserValue> SimpleMatcherCharacter::CanMatch(ILexer<SimpleParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);
    return token.m_type == SimpleParserValueType::CHARACTER && token.CharacterValue() == m_char
        ? MatcherResult<SimpleParserValue>::Match(1)
        : MatcherResult<SimpleParserValue>::NoMatch();
}
