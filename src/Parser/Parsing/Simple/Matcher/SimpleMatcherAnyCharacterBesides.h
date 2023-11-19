#pragma once

#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Simple/SimpleParserValue.h"

#include <vector>

class SimpleMatcherAnyCharacterBesides final : public AbstractMatcher<SimpleParserValue>
{
    std::vector<char> m_chars;

protected:
    MatcherResult<SimpleParserValue> CanMatch(ILexer<SimpleParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit SimpleMatcherAnyCharacterBesides(std::vector<char> chars);
};
