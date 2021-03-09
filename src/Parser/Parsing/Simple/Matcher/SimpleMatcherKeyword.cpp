#include "SimpleMatcherKeyword.h"

SimpleMatcherKeyword::SimpleMatcherKeyword(std::string value)
    : m_value(std::move(value))
{
    const std::hash<std::string> hash;
    m_hash = hash(m_value);
}

MatcherResult<SimpleParserValue> SimpleMatcherKeyword::CanMatch(ILexer<SimpleParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);
    return token.m_type == SimpleParserValueType::IDENTIFIER && token.IdentifierHash() == m_hash && token.IdentifierValue() == m_value
        ? MatcherResult<SimpleParserValue>::Match(1)
        : MatcherResult<SimpleParserValue>::NoMatch();
}
