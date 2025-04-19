#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceSetBlock final : public CommandsParser::sequence_t
{
public:
    SequenceSetBlock();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
