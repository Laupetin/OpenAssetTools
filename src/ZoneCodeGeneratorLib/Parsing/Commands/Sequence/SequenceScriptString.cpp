#include "SequenceScriptString.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

SequenceScriptString::SequenceScriptString()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("scriptstring"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Char(';'),
    });
}

void SequenceScriptString::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);

    StructureInformation* type;
    std::vector<MemberInformation*> members;
    if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), type, members))
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    if (members.empty())
        throw ParsingException(typeNameToken.GetPos(), "Need to specify a member when trying to mark as script string.");

    auto* lastMember = members.back();
    const auto* dataDef = lastMember->m_member->m_type_declaration->m_type;
    while (dataDef->GetType() == DataDefinitionType::TYPEDEF)
        dataDef = dynamic_cast<const TypedefDefinition*>(dataDef)->m_type_declaration->m_type;

    if (dataDef->GetType() != DataDefinitionType::BASE_TYPE)
        throw ParsingException(typeNameToken.GetPos(), "Invalid type for script string, must be a base type");

    members.back()->m_is_script_string = true;
}
