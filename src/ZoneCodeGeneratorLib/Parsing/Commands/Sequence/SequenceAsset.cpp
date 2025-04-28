#include "SequenceAsset.h"

#include "Parsing/Commands/Matcher/CommandsCommonMatchers.h"
#include "Parsing/Commands/Matcher/CommandsMatcherFactory.h"

namespace
{
    static constexpr auto CAPTURE_TYPE = 1;
    static constexpr auto CAPTURE_ASSET_NAME = 2;
} // namespace

SequenceAsset::SequenceAsset()
{
    const CommandsMatcherFactory create(this);

    AddLabeledMatchers(CommandsCommonMatchers::Typename(this), CommandsCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Keyword("asset"),
        create.Label(CommandsCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_TYPE),
        create.Identifier().Capture(CAPTURE_ASSET_NAME),
        create.Char(';'),
    });
}

void SequenceAsset::ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE);
    const auto& assetNameToken = result.NextCapture(CAPTURE_ASSET_NAME);

    auto* definition = state->GetRepository()->GetDataDefinitionByName(typeNameToken.TypeNameValue());
    if (definition == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "Unknown type");

    auto* definitionWithMembers = dynamic_cast<DefinitionWithMembers*>(definition);
    if (definitionWithMembers == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "Type must be struct or union");

    auto* information = state->GetRepository()->GetInformationFor(definitionWithMembers);
    if (information == nullptr)
        throw ParsingException(typeNameToken.GetPos(), "No information for definition");

    information->m_asset_name = assetNameToken.IdentifierValue();
    if (information->m_asset_name.empty())
        throw ParsingException(assetNameToken.GetPos(), "Asset name is empty");
}
