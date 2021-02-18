#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceAction final : public CommandsParser::sequence_t
{
    static constexpr auto CAPTURE_ACTION_NAME = 1;
    static constexpr auto CAPTURE_TYPE = 2;
    static constexpr auto CAPTURE_ARG_TYPE = 3;

    static constexpr auto LABEL_ACTION_ARGS = 1;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceAction();
};
