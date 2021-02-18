#include "CommandsMatcherCharacter.h"

CommandsMatcherCharacter::CommandsMatcherCharacter(const char c)
    : m_char(c)
{
}

MatcherResult<CommandsParserValue> CommandsMatcherCharacter::CanMatch(ILexer<CommandsParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);
    return token.m_type == CommandsParserValueType::CHARACTER && token.CharacterValue() == m_char
        ? MatcherResult<CommandsParserValue>::Match(1)
        : MatcherResult<CommandsParserValue>::NoMatch();
}
