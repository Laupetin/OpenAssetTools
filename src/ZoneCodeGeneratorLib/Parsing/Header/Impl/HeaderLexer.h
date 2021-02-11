#pragma once

#include "HeaderParserValue.h"
#include "Parsing/AbstractLexer.h"

class HeaderLexer final : public AbstractLexer<HeaderParserValue>
{
protected:
    HeaderParserValue GetNextToken() override;

public:
    explicit HeaderLexer(IParserLineStream* stream);
};
