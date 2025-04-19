#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceArraySize final : public CommandsParser::sequence_t
{
public:
    SequenceArraySize();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
