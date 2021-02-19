#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceSetBlock final : public CommandsParser::sequence_t
{
    static constexpr auto CAPTURE_START = 1;
    static constexpr auto CAPTURE_TYPE = 2;
    static constexpr auto CAPTURE_BLOCK_ENUM_ENTRY = 3;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceSetBlock();
};
