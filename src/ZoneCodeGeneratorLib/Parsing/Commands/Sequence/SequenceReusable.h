#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceReusable final : public CommandsParser::sequence_t
{
public:
    SequenceReusable();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
