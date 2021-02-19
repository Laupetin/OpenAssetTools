#pragma once

#include "Domain/Definition/PointerDeclarationModifier.h"
#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceCount final : public CommandsParser::sequence_t
{
    static constexpr auto TAG_POINTER_RESOLVE = 1;

    static constexpr auto CAPTURE_START = 1;
    static constexpr auto CAPTURE_TYPE = 2;
    static constexpr auto CAPTURE_ARRAY_INDEX = 3;

    static void SetCountByArrayIndex(CommandsParserState* state, SequenceResult<CommandsParserValue>& result, MemberInformation* member, PointerDeclarationModifier* pointer,
                                     std::unique_ptr<IEvaluation> evaluation);

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceCount();
};
