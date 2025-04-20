#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceName final : public CommandsParser::sequence_t
{
public:
    SequenceName();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
