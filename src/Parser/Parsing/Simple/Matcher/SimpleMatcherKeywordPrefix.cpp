#include "SimpleMatcherKeywordPrefix.h"

SimpleMatcherKeywordPrefix::SimpleMatcherKeywordPrefix(std::string value)
    : m_value(std::move(value))
{
}

MatcherResult<SimpleParserValue> SimpleMatcherKeywordPrefix::CanMatch(ILexer<SimpleParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);
    return token.m_type == SimpleParserValueType::IDENTIFIER && token.IdentifierValue().compare(0, m_value.size(), m_value) == 0
               ? MatcherResult<SimpleParserValue>::Match(1)
               : MatcherResult<SimpleParserValue>::NoMatch();
}
