#include "SequenceAllocAlign.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

namespace
{
    static constexpr auto CAPTURE_TYPE = 1;
}

SequenceAllocAlign::SequenceAllocAlign()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddLabeledMatchers(CommandsCommonMatchers::Evaluation(this), CommandsCommonMatchers::LABEL_EVALUATION);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("allocalign"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Label(CommandsCommonMatchers::LABEL_EVALUATION),
        create.Char(';'),
    });
}

void SequenceAllocAlign::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);
    StructureInformation* type;
    std::vector<MemberInformation*> memberChain;
    if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), type, memberChain))
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    auto allocAlignEvaluation = CommandsCommonMatchers::ProcessEvaluation(state, result, type);
    if (memberChain.empty())
        type->m_alloc_alignment = std::move(allocAlignEvaluation);
    else
        memberChain.back()->m_alloc_alignment = std::move(allocAlignEvaluation);
}
