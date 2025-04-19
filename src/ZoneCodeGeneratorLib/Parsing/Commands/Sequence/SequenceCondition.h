#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceCondition final : public CommandsParser::sequence_t
{
public:
    SequenceCondition();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
