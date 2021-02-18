#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceCount final : public CommandsParser::sequence_t
{
    static constexpr auto TAG_POINTER_RESOLVE = 1;

    static constexpr auto CAPTURE_TYPE = 1;
    static constexpr auto CAPTURE_ARRAY_INDEX = 2;
    static constexpr auto CAPTURE_EVALUATION = 3;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceCount();
};
