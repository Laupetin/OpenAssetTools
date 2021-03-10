#pragma once

#include <vector>

#include "Parsing/Simple/SimpleParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class SimpleMatcherAnyCharacterBesides final : public AbstractMatcher<SimpleParserValue>
{
    std::vector<char> m_chars;

protected:
    MatcherResult<SimpleParserValue> CanMatch(ILexer<SimpleParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit SimpleMatcherAnyCharacterBesides(std::vector<char> chars);
};
