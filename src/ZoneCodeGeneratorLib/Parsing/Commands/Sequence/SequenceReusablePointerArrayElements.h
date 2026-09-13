#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceReusablePointerArrayElements final : public CommandsParser::sequence_t
{
public:
    SequenceReusablePointerArrayElements();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
