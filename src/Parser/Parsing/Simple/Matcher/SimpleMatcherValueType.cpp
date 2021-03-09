#include "SimpleMatcherValueType.h"

SimpleMatcherValueType::SimpleMatcherValueType(const SimpleParserValueType type)
    : m_type(type)
{
}

MatcherResult<SimpleParserValue> SimpleMatcherValueType::CanMatch(ILexer<SimpleParserValue>* lexer, const unsigned tokenOffset)
{
    return lexer->GetToken(tokenOffset).m_type == m_type
        ? MatcherResult<SimpleParserValue>::Match(1)
        : MatcherResult<SimpleParserValue>::NoMatch();
}
