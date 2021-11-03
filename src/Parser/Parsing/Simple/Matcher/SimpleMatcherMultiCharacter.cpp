#include "SimpleMatcherMultiCharacter.h"

SimpleMatcherMultiCharacter::SimpleMatcherMultiCharacter(const int multiCharacterSequenceId)
    : m_multi_character_sequence_id(multiCharacterSequenceId)
{
}

MatcherResult<SimpleParserValue> SimpleMatcherMultiCharacter::CanMatch(ILexer<SimpleParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);
    return token.m_type == SimpleParserValueType::MULTI_CHARACTER && token.MultiCharacterValue() == m_multi_character_sequence_id
               ? MatcherResult<SimpleParserValue>::Match(1)
               : MatcherResult<SimpleParserValue>::NoMatch();
}
