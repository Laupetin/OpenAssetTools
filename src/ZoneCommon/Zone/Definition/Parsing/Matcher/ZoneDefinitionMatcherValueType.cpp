#include "ZoneDefinitionMatcherValueType.h"

ZoneDefinitionMatcherValueType::ZoneDefinitionMatcherValueType(const ZoneDefinitionParserValueType type)
    : m_type(type)
{
}

MatcherResult<ZoneDefinitionParserValue> ZoneDefinitionMatcherValueType::CanMatch(ILexer<ZoneDefinitionParserValue>* lexer, const unsigned tokenOffset)
{
    return lexer->GetToken(tokenOffset).m_type == m_type ? MatcherResult<ZoneDefinitionParserValue>::Match(1)
                                                         : MatcherResult<ZoneDefinitionParserValue>::NoMatch();
}
