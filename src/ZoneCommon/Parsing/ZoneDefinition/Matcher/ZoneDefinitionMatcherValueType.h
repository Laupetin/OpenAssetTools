#pragma once

#include "Parsing/ZoneDefinition/ZoneDefinitionParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class ZoneDefinitionMatcherValueType final : public AbstractMatcher<ZoneDefinitionParserValue>
{
    ZoneDefinitionParserValueType m_type;

protected:
    MatcherResult<ZoneDefinitionParserValue> CanMatch(ILexer<ZoneDefinitionParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit ZoneDefinitionMatcherValueType(ZoneDefinitionParserValueType type);
};
