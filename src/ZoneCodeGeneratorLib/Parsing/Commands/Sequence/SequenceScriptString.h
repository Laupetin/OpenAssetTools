#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceScriptString final : public CommandsParser::sequence_t
{
public:
    SequenceScriptString();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
