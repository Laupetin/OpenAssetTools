#pragma once

#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Simple/SimpleParserValue.h"

class SimpleMatcherCharacter final : public AbstractMatcher<SimpleParserValue>
{
    char m_char;

protected:
    MatcherResult<SimpleParserValue> CanMatch(ILexer<SimpleParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit SimpleMatcherCharacter(char c);
};
