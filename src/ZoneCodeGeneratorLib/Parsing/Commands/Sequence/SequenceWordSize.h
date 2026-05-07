#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceWordSize final : public CommandsParser::sequence_t
{
public:
    SequenceWordSize();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
