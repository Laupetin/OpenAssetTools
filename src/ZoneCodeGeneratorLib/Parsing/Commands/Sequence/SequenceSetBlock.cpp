#include "SequenceSetBlock.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

SequenceSetBlock::SequenceSetBlock()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("set").Capture(CAPTURE_START),
        create.Keyword("block"),
        create.Or({
            create.And({
                create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
                create.Identifier().Capture(CAPTURE_BLOCK_ENUM_ENTRY),
            }),
            create.Identifier().Capture(CAPTURE_BLOCK_ENUM_ENTRY),
        }),
        create.Char(';'),
    });
}

void SequenceSetBlock::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    StructureInformation* information;
    std::vector<MemberInformation*> memberChain;

    if (result.HasNextCapture(CAPTURE_TYPE))
    {
        const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);
        if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), information, memberChain))
            throw ParsingException(typeNameToken.GetPos(), "Unknown type");
    }
    else if (state->GetInUse() != nullptr)
    {
        information = state->GetInUse();
    }
    else
        throw ParsingException(result.NextCapture(CAPTURE_START).GetPos(), "No type is used. Therefore one needs to be specified directly.");

    const auto& blockNameToken = result.NextCapture(CAPTURE_BLOCK_ENUM_ENTRY);
    auto* fastFileBlock = state->GetRepository()->GetFastFileBlockByName(blockNameToken.IdentifierValue());
    if (fastFileBlock == nullptr)
        throw ParsingException(blockNameToken.GetPos(), "Unknown block");

    if (!memberChain.empty())
        memberChain.back()->m_fast_file_block = fastFileBlock;
    else
        information->m_block = fastFileBlock;
}
