#pragma once

#include "Domain/Definition/PointerDeclarationModifier.h"
#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceCount final : public CommandsParser::sequence_t
{
public:
    SequenceCount();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;
};
