#include "CommandsMatcherValueType.h"

CommandsMatcherValueType::CommandsMatcherValueType(CommandsParserValueType type)
    : m_type(type)
{
}

MatcherResult<CommandsParserValue> CommandsMatcherValueType::CanMatch(ILexer<CommandsParserValue>* lexer, const unsigned tokenOffset)
{
    return lexer->GetToken(tokenOffset).m_type == m_type
        ? MatcherResult<CommandsParserValue>::Match(1)
        : MatcherResult<CommandsParserValue>::NoMatch();
}
