#include "CommandsMatcherKeyword.h"

CommandsMatcherKeyword::CommandsMatcherKeyword(std::string value)
    : m_value(std::move(value))
{
    const std::hash<std::string> hash;
    m_hash = hash(m_value);
}

MatcherResult<CommandsParserValue> CommandsMatcherKeyword::CanMatch(ILexer<CommandsParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);
    return token.m_type == CommandsParserValueType::IDENTIFIER && token.IdentifierHash() == m_hash && token.IdentifierValue() == m_value
               ? MatcherResult<CommandsParserValue>::Match(1)
               : MatcherResult<CommandsParserValue>::NoMatch();
}
