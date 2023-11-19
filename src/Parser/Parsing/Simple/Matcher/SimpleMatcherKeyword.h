#pragma once

#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Simple/SimpleParserValue.h"

#include <string>

class SimpleMatcherKeyword final : public AbstractMatcher<SimpleParserValue>
{
    size_t m_hash;
    std::string m_value;

protected:
    MatcherResult<SimpleParserValue> CanMatch(ILexer<SimpleParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit SimpleMatcherKeyword(std::string value);
};
