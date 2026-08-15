#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceVariant final : public CommandsParser::sequence_t
{
public:
    SequenceVariant();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
