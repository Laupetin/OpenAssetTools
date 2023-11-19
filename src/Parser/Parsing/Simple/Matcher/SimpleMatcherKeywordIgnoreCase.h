#pragma once

#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Simple/SimpleParserValue.h"

#include <string>

class SimpleMatcherKeywordIgnoreCase final : public AbstractMatcher<SimpleParserValue>
{
    std::string m_value;

protected:
    MatcherResult<SimpleParserValue> CanMatch(ILexer<SimpleParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit SimpleMatcherKeywordIgnoreCase(std::string value);
};
