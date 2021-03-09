#pragma once

#include "SimpleParserValue.h"
#include "Parsing/Impl/AbstractLexer.h"

class SimpleLexer final : public AbstractLexer<SimpleParserValue>
{
protected:
    SimpleParserValue GetNextToken() override;

public:
    explicit SimpleLexer(IParserLineStream* stream);
};