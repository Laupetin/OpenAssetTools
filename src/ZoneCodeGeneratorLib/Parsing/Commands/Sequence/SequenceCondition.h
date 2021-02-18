#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceCondition final : public CommandsParser::sequence_t
{
    static constexpr auto TAG_ALWAYS = 1;
    static constexpr auto TAG_NEVER = 2;
    static constexpr auto TAG_EVALUATION = 3;

    static constexpr auto CAPTURE_TYPE = 1;
    static constexpr auto CAPTURE_EVALUATION = 2;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceCondition();
};
