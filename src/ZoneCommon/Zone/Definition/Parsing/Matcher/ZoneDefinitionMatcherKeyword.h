#pragma once

#include "Parsing/Matcher/AbstractMatcher.h"
#include "Zone/Definition/Parsing/ZoneDefinitionParserValue.h"

#include <string>

class ZoneDefinitionMatcherKeyword final : public AbstractMatcher<ZoneDefinitionParserValue>
{
    size_t m_hash;
    std::string m_value;

protected:
    MatcherResult<ZoneDefinitionParserValue> CanMatch(ILexer<ZoneDefinitionParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit ZoneDefinitionMatcherKeyword(std::string value);
};
