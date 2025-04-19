#pragma once

#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class HeaderMatcherValueType final : public AbstractMatcher<HeaderParserValue>
{
public:
    explicit HeaderMatcherValueType(HeaderParserValueType type);

protected:
    MatcherResult<HeaderParserValue> CanMatch(ILexer<HeaderParserValue>* lexer, unsigned tokenOffset) override;

private:
    HeaderParserValueType m_type;
};
