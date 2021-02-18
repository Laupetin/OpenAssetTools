#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceArraySize final : public CommandsParser::sequence_t
{
    static constexpr auto CAPTURE_TYPE = 1;
    static constexpr auto CAPTURE_EVALUATION = 2;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceArraySize();
};
