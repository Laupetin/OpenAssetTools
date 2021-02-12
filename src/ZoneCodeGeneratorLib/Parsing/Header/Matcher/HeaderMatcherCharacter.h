#pragma once

#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class HeaderMatcherCharacter final : public AbstractMatcher<HeaderParserValue>
{
    char m_char;

protected:
    MatcherResult<HeaderParserValue> CanMatch(AbstractLexer<HeaderParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit HeaderMatcherCharacter(char c);
};
