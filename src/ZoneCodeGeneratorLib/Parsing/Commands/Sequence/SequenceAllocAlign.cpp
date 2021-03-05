#include "SequenceAllocAlign.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

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
        create.Char(';')
        });
}

void SequenceAllocAlign::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);
    StructureInformation* type;
    std::vector<MemberInformation*> memberChain;
    if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), type, memberChain))
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    if(memberChain.empty())
        throw ParsingException(typeNameToken.GetPos(), "Need to specify a member");

    auto allocAlignEvaluation = CommandsCommonMatchers::ProcessEvaluation(state, result, type);
    memberChain.back()->m_alloc_alignment = std::move(allocAlignEvaluation);
}
