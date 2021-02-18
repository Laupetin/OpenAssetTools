#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceAsset final : public CommandsParser::sequence_t
{
    static constexpr auto CAPTURE_TYPE = 1;
    static constexpr auto CAPTURE_ENUM_ENTRY = 2;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceAsset();
};
