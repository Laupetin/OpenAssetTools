#include "SequenceAsset.h"

#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"
#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"

SequenceAsset::SequenceAsset()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("asset"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Identifier().Capture(CAPTURE_ENUM_ENTRY),
        create.Char(';')
    });
}

void SequenceAsset::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);
    const auto& enumEntryToken = result.NextCapture(CAPTURE_ENUM_ENTRY);

    auto* definition = state->GetRepository()->GetDataDefinitionByName(typeNameToken.TypeNameValue());
    if (definition == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    auto* definitionWithMembers = dynamic_cast<DefinitionWithMembers*>(definition);
    if (definitionWithMembers == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "Type must be struct or union");

    auto* information = state->GetRepository()->GetInformationFor(definitionWithMembers);
    if (information == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "No information for definition");

    auto* enumMember = state->GetRepository()->GetEnumMemberByName(enumEntryToken.IdentifierValue());
    if (enumMember == nullptr)
        throw ParsingException(enumEntryToken.GetPos(), "Unknown enum entry");

    information->m_asset_enum_entry = enumMember;
}
