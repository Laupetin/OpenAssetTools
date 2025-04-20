#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceAction final : public CommandsParser::sequence_t
{
public:
    SequenceAction();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
