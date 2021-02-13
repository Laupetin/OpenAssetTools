#pragma once

#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class HeaderMatcherValueType final : public AbstractMatcher<HeaderParserValue>
{
    HeaderParserValueType m_type;

protected:
    MatcherResult<HeaderParserValue> CanMatch(ILexer<HeaderParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit HeaderMatcherValueType(HeaderParserValueType type);
};
