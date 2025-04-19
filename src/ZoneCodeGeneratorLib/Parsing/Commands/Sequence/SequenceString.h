#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceString final : public CommandsParser::sequence_t
{
public:
    SequenceString();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
