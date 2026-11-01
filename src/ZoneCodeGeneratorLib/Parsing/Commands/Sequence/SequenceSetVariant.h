#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceSetVariant final : public CommandsParser::sequence_t
{
public:
    SequenceSetVariant();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
