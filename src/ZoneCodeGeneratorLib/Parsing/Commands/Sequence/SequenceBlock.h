#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceBlock final : public CommandsParser::sequence_t
{
    static constexpr auto TAG_DEFAULT = 1;

    static constexpr auto CAPTURE_BLOCK_ENUM_ENTRY = 1;
    static constexpr auto CAPTURE_BLOCK_TYPE = 2;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceBlock();
};
