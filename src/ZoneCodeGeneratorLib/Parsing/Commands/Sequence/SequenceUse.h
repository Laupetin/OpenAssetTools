#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceUse final : public CommandsParser::sequence_t
{
    static constexpr auto CAPTURE_TYPE = 1;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceUse();
};
