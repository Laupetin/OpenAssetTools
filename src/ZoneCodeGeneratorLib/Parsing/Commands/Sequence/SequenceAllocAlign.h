#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceAllocAlign final : public CommandsParser::sequence_t
{
public:
    SequenceAllocAlign();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
