#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceSetAliasWordSize final : public CommandsParser::sequence_t
{
public:
    SequenceSetAliasWordSize();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
