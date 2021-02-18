#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceGame final : public CommandsParser::sequence_t
{
    static constexpr auto CAPTURE_GAME = 1;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceGame();
};
