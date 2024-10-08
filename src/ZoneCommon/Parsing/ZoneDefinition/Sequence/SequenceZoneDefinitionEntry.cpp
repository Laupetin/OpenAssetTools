#include "SequenceZoneDefinitionEntry.h"

#include "Parsing/ZoneDefinition/Matcher/ZoneDefinitionMatcherFactory.h"

SequenceZoneDefinitionEntry::SequenceZoneDefinitionEntry()
{
    const ZoneDefinitionMatcherFactory create(this);

    AddMatchers({
        create.Field().Capture(CAPTURE_TYPE_NAME),
        create.Char(','),
        create.Optional(create.Char(',').Tag(TAG_REFERENCE)),
        create
            .Or({
                create.String(),
                create.Field(),
            })
            .Capture(CAPTURE_ASSET_NAME),
    });
}

void SequenceZoneDefinitionEntry::ProcessMatch(ZoneDefinitionParserState* state, SequenceResult<ZoneDefinitionParserValue>& result) const
{
    const auto& typeNameToken = result.NextCapture(CAPTURE_TYPE_NAME);

    if (!state->m_asset_name_resolver)
        throw ParsingException(typeNameToken.GetPos(), "Must define game before first asset");

    const auto maybeAssetType = state->m_asset_name_resolver->GetAssetTypeByName(typeNameToken.FieldValue());
    if (!maybeAssetType)
        throw ParsingException(typeNameToken.GetPos(), "Unknown asset type");

    const auto& assetNameToken = result.NextCapture(CAPTURE_ASSET_NAME);
    std::string assetName;
    if (assetNameToken.m_type == ZoneDefinitionParserValueType::STRING)
        assetName = assetNameToken.StringValue();
    else
        assetName = assetNameToken.FieldValue();

    state->m_definition->m_assets.emplace_back(*maybeAssetType, assetName, result.NextTag() == TAG_REFERENCE);
}
