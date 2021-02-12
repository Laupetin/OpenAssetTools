#include "HeaderMatcherValueType.h"

HeaderMatcherValueType::HeaderMatcherValueType(HeaderParserValueType type)
    : m_type(type)
{
}

MatcherResult<HeaderParserValue> HeaderMatcherValueType::CanMatch(AbstractLexer<HeaderParserValue>* lexer, const unsigned tokenOffset)
{
    return lexer->GetToken(tokenOffset).m_type == m_type
               ? MatcherResult<HeaderParserValue>::Match(1)
               : MatcherResult<HeaderParserValue>::NoMatch();
}
