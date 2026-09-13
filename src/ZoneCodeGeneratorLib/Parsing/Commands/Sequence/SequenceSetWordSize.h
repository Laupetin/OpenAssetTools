#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceSetWordSize final : public CommandsParser::sequence_t
{
public:
    SequenceSetWordSize();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
