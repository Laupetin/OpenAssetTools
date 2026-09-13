#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequencePointerArrayWordSize final : public CommandsParser::sequence_t
{
public:
    SequencePointerArrayWordSize();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
