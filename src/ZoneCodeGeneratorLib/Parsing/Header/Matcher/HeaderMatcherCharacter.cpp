#include "HeaderMatcherCharacter.h"

HeaderMatcherCharacter::HeaderMatcherCharacter(const char c)
    : m_char(c)
{
}

MatcherResult<HeaderParserValue> HeaderMatcherCharacter::CanMatch(ILexer<HeaderParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);
    return token.m_type == HeaderParserValueType::CHARACTER && token.CharacterValue() == m_char
               ? MatcherResult<HeaderParserValue>::Match(1)
               : MatcherResult<HeaderParserValue>::NoMatch();
}
