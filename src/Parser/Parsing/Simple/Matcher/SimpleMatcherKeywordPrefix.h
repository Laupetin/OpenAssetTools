#pragma once

#include <string>

#include "Parsing/Simple/SimpleParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class SimpleMatcherKeywordPrefix final : public AbstractMatcher<SimpleParserValue>
{
    std::string m_value;

protected:
    MatcherResult<SimpleParserValue> CanMatch(ILexer<SimpleParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit SimpleMatcherKeywordPrefix(std::string value);
};
