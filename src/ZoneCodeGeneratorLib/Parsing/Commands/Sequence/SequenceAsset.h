#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceAsset final : public CommandsParser::sequence_t
{
public:
    SequenceAsset();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
