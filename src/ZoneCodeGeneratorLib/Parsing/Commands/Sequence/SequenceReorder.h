#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceReorder final : public CommandsParser::sequence_t
{
    static constexpr auto TAG_FIND_FIRST = 1;

    static constexpr auto CAPTURE_START = 1;
    static constexpr auto CAPTURE_TYPE = 2;
    static constexpr auto CAPTURE_ENTRY = 3;

    static StructureInformation* GetType(CommandsParserState* state, SequenceResult<CommandsParserValue>& result);

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceReorder();
};
