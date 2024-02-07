#include "SequenceAssetRef.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

#include <algorithm>

SequenceAssetRef::SequenceAssetRef()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("set"),
        create.Keyword("assetref"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Identifier().Capture(CAPTURE_ASSET_TYPE_ENUM_ENTRY),
        create.Char(';'),
    });
}

void SequenceAssetRef::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& enumEntryToken = result.NextCapture(CAPTURE_ASSET_TYPE_ENUM_ENTRY);
    const auto* enumMember = state->GetRepository()->GetEnumMemberByName(enumEntryToken.IdentifierValue());
    if (enumMember == nullptr)
        throw ParsingException(enumEntryToken.GetPos(), "Unknown asset type enum entry");

    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);

    StructureInformation* type;
    std::vector<MemberInformation*> members;
    if (!state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), type, members))
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    if (members.empty())
        throw ParsingException(typeNameToken.GetPos(), "Need to specify a member when trying to mark as string.");

    auto* lastMember = members.back();
    const auto* typeDecl = lastMember->m_member->m_type_declaration.get();
    auto hasPointerRef = false;

    while (true)
    {
        if (!hasPointerRef)
        {
            const auto& modifiers = typeDecl->m_declaration_modifiers;
            hasPointerRef = std::any_of(modifiers.begin(),
                                        modifiers.end(),
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

    lastMember->m_asset_ref = enumMember;
}
