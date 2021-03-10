#pragma once

#include "Parsing/Simple/SimpleLexer.h"

class ZoneDefinitionLexer final : public SimpleLexer
{
public:
    explicit ZoneDefinitionLexer(IParserLineStream* stream);
};