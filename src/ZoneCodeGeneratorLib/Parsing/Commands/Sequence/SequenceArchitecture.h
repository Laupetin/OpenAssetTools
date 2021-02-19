#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceArchitecture final : public CommandsParser::sequence_t
{
    static constexpr auto TAG_X86 = 1;
    static constexpr auto TAG_X64 = 2;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceArchitecture();
};
