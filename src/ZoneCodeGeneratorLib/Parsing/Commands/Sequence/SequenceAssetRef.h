#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceAssetRef final : public CommandsParser::sequence_t
{
public:
    SequenceAssetRef();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
