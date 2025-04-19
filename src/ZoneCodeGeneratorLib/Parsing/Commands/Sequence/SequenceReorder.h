#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceReorder final : public CommandsParser::sequence_t
{
public:
    SequenceReorder();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
