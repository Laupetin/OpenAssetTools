#pragma once

#include "Parsing/Simple/SimpleParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class SimpleMatcherValueTypeAndHasSignPrefix final : public AbstractMatcher<SimpleParserValue>
{
    SimpleParserValueType m_type;
    bool m_has_sign_prefix;

protected:
    MatcherResult<SimpleParserValue> CanMatch(ILexer<SimpleParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit SimpleMatcherValueTypeAndHasSignPrefix(SimpleParserValueType type, bool hasSignPrefix);
};
