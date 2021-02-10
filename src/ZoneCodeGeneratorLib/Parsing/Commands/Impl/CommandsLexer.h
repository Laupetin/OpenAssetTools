#pragma once

#include "CommandsParserValue.h"
#include "Parsing/AbstractLexer.h"

class CommandsLexer final : public AbstractLexer<CommandsParserValue>
{
protected:
    CommandsParserValue GetNextToken() override;

public:
    explicit CommandsLexer(IParserLineStream* stream);
};
