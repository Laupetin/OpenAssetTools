#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceArrayCount final : public CommandsParser::sequence_t
{
public:
    SequenceArrayCount();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
