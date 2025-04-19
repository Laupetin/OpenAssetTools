#include "SequenceString.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

#include <algorithm>

namespace
{
    static constexpr auto CAPTURE_TYPE = 1;
}

SequenceString::SequenceString()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("string"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Char(';'),
    });
}

void SequenceString::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);

    StructureInformation* type;
    std::vector<MemberInformation*> members;
    if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), type, members))
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    if (members.empty())
        throw ParsingException(typeNameToken.GetPos(), "Need to specify a member when trying to mark as string.");

    auto* lastMember = members.back();
    auto* typeDecl = lastMember->m_member->m_type_declaration.get();
    auto hasPointerRef = false;

    while (true)
    {
        if (!hasPointerRef)
        {
            const auto& modifiers = typeDecl->m_declaration_modifiers;
            hasPointerRef = std::ranges::any_of(modifiers,
                                                [](const std::unique_ptr<DeclarationModifier>& modifier)
                                                {
                                                    return modifier->GetType() == DeclarationModifierType::POINTER;
                                                });
        }

        if (typeDecl->m_type->GetType() == DataDefinitionType::TYPEDEF)
        {
            const auto* typedefDef = dynamic_cast<const TypedefDefinition*>(typeDecl->m_type);
            typeDecl = typedefDef->m_type_declaration.get();
        }
        else
            break;
    }

    if (!hasPointerRef)
        throw ParsingException(typeNameToken.GetPos(), "Invalid type for string, must be a pointer");

    if (typeDecl->m_type->GetType() != DataDefinitionType::BASE_TYPE)
        throw ParsingException(typeNameToken.GetPos(), "Invalid type for string, must be a base type");

    lastMember->m_is_string = true;
}
