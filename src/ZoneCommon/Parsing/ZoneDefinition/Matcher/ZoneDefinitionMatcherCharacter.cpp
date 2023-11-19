#include "ZoneDefinitionMatcherCharacter.h"

ZoneDefinitionMatcherCharacter::ZoneDefinitionMatcherCharacter(const char c)
    : m_char(c)
{
}

MatcherResult<ZoneDefinitionParserValue> ZoneDefinitionMatcherCharacter::CanMatch(ILexer<ZoneDefinitionParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);
    return token.m_type == ZoneDefinitionParserValueType::CHARACTER && token.CharacterValue() == m_char ? MatcherResult<ZoneDefinitionParserValue>::Match(1)
                                                                                                        : MatcherResult<ZoneDefinitionParserValue>::NoMatch();
}
