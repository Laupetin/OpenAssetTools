#pragma once

#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class HeaderMatcherCharacter final : public AbstractMatcher<HeaderParserValue>
{
public:
    explicit HeaderMatcherCharacter(char c);

protected:
    MatcherResult<HeaderParserValue> CanMatch(ILexer<HeaderParserValue>* lexer, unsigned tokenOffset) override;

private:
    char m_char;
};
