#pragma once

#include "CommandsParserValue.h"
#include "Parsing/Impl/AbstractLexer.h"

class CommandsLexer final : public AbstractLexer<CommandsParserValue>
{
public:
    explicit CommandsLexer(IParserLineStream* stream);

protected:
    CommandsParserValue GetNextToken() override;
};
