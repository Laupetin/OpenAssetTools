#pragma once

#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Simple/SimpleParserValue.h"

class MenuMatcherScriptNumeric final : public AbstractMatcher<SimpleParserValue>
{
protected:
    MatcherResult<SimpleParserValue> CanMatch(ILexer<SimpleParserValue>* lexer, unsigned tokenOffset) override;

public:
    MenuMatcherScriptNumeric();
};
