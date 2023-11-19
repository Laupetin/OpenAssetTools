#include "SimpleMatcherValueTypeAndHasSignPrefix.h"

SimpleMatcherValueTypeAndHasSignPrefix::SimpleMatcherValueTypeAndHasSignPrefix(const SimpleParserValueType type, bool hasSignPrefix)
    : m_type(type),
      m_has_sign_prefix(hasSignPrefix)
{
}

MatcherResult<SimpleParserValue> SimpleMatcherValueTypeAndHasSignPrefix::CanMatch(ILexer<SimpleParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);
    return token.m_type == m_type && token.m_has_sign_prefix == m_has_sign_prefix ? MatcherResult<SimpleParserValue>::Match(1)
                                                                                  : MatcherResult<SimpleParserValue>::NoMatch();
}
