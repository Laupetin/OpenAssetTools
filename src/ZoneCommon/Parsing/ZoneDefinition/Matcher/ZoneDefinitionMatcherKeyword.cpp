#include "ZoneDefinitionMatcherKeyword.h"

ZoneDefinitionMatcherKeyword::ZoneDefinitionMatcherKeyword(std::string value)
    : m_value(std::move(value))
{
    const std::hash<std::string> hash;
    m_hash = hash(m_value);
}

MatcherResult<ZoneDefinitionParserValue> ZoneDefinitionMatcherKeyword::CanMatch(ILexer<ZoneDefinitionParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);
    return token.m_type == ZoneDefinitionParserValueType::FIELD && token.FieldHash() == m_hash && token.FieldValue() == m_value
               ? MatcherResult<ZoneDefinitionParserValue>::Match(1)
               : MatcherResult<ZoneDefinitionParserValue>::NoMatch();
}
