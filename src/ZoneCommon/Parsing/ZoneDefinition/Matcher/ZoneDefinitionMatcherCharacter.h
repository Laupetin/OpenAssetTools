#pragma once

#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/ZoneDefinition/ZoneDefinitionParserValue.h"

class ZoneDefinitionMatcherCharacter final : public AbstractMatcher<ZoneDefinitionParserValue>
{
    char m_char;

protected:
    MatcherResult<ZoneDefinitionParserValue> CanMatch(ILexer<ZoneDefinitionParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit ZoneDefinitionMatcherCharacter(char c);
};
