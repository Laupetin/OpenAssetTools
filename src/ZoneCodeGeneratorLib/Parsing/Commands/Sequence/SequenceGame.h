#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceGame final : public CommandsParser::sequence_t
{
public:
    SequenceGame();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
