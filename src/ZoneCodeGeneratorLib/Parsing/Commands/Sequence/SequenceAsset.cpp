#include "SequenceAsset.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

namespace
{
    constexpr auto CAPTURE_START = 1;
    constexpr auto CAPTURE_TYPE = 2;
    constexpr auto CAPTURE_ASSET_NAME = 3;
} // namespace

SequenceAsset::SequenceAsset()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("asset").Capture(CAPTURE_START),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Optional(create.Identifier().Capture(CAPTURE_ASSET_NAME)),
        create.Char(';'),
    });
}

void SequenceAsset::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);

    auto* definition = state->GetRepository()->GetDataDefinitionByName(typeNameToken.TypeNameValue());
    if (definition == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    const auto* definitionWithMembers = dynamic_cast<DefinitionWithMembers*>(definition);
    if (definitionWithMembers == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "Type must be struct or union");

    auto* information = state->GetRepository()->GetInformationFor(definitionWithMembers);
    if (information == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "No information for definition");

    if (result.HasNextCapture(CAPTURE_ASSET_NAME))
    {
        const auto& assetNameToken = result.NextCapture(CAPTURE_ASSET_NAME);
        information->m_asset_name = assetNameToken.IdentifierValue();
    }
    else
    {
        information->m_asset_name = definition->m_name;
    }

    if (information->m_asset_name.empty())
        throw ParsingException(result.NextCapture(CAPTURE_START).GetPos(), "Asset name is empty");
}
