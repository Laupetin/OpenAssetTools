#include "SequenceReusable.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

namespace
{
    static constexpr auto CAPTURE_TYPE = 1;
}

SequenceReusable::SequenceReusable()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("reusable"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Char(';'),
    });
}

void SequenceReusable::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);

    StructureInformation* type;
    std::vector<MemberInformation*> members;
    if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), type, members))
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    if (members.empty())
        throw ParsingException(typeNameToken.GetPos(), "Need to specify a member when trying to mark as reusable.");

    members.back()->m_is_reusable = true;
}
