#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceUse final : public CommandsParser::sequence_t
{
public:
    SequenceUse();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
