#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceReorder final : public CommandsParser::sequence_t
{
    static constexpr auto TAG_FIND_FIRST = 1;

    static constexpr auto CAPTURE_TYPE = 1;
    static constexpr auto CAPTURE_ENTRY = 2;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceReorder();
};
