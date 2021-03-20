#include "SequenceZoneDefinitionEntry.h"

#include "Parsing/ZoneDefinition/Matcher/ZoneDefinitionMatcherFactory.h"

SequenceZoneDefinitionEntry::SequenceZoneDefinitionEntry()
{
    const ZoneDefinitionMatcherFactory create(this);
    
    AddMatchers({
        create.Field().Capture(CAPTURE_TYPE_NAME),
        create.Char(','),
        create.Optional(create.Char(',').Tag(TAG_REFERENCE)),
        create.Or({
            create.String(),
            create.Field()
        }).Capture(CAPTURE_ASSET_NAME)
    });
}

void SequenceZoneDefinitionEntry::ProcessMatch(ZoneDefinition* state, SequenceResult<ZoneDefinitionParserValue>& result) const
{
    const auto& assetNameCapture = result.NextCapture(CAPTURE_ASSET_NAME);

    std::string assetName;
    if (assetNameCapture.m_type == ZoneDefinitionParserValueType::STRING)
    {
        assetName = assetNameCapture.StringValue();
    }
    else
    {
        assetName = assetNameCapture.FieldValue();
    }

    state->m_assets.emplace_back(
        result.NextCapture(CAPTURE_TYPE_NAME).FieldValue(),
        assetName,
        result.NextTag() == TAG_REFERENCE);
}
