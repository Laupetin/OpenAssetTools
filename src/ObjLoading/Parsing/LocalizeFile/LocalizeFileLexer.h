#pragma once

#include "Parsing/Simple/SimpleLexer.h"

class LocalizeFileLexer final : public SimpleLexer
{
public:
    explicit LocalizeFileLexer(IParserLineStream* stream);
};
