#include "SequenceZoneDefinitionEntry.h"

#include "Parsing/ZoneDefinition/Matcher/ZoneDefinitionCommonMatchers.h"
#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceZoneDefinitionEntry::SequenceZoneDefinitionEntry()
{
    const SimpleMatcherFactory create(this);

    AddLabeledMatchers(ZoneDefinitionCommonMatchers::AssetName(this), ZoneDefinitionCommonMatchers::LABEL_ASSET_NAME);
    AddMatchers({
        create.Identifier().Capture(CAPTURE_TYPE_NAME),
        create.Char(','),
        create.Optional(create.Char(',').Tag(TAG_REFERENCE)),
        create.Label(ZoneDefinitionCommonMatchers::LABEL_ASSET_NAME).Capture(CAPTURE_ASSET_NAME),
        create.Or({
            create.Type(SimpleParserValueType::NEW_LINE),
            create.Type(SimpleParserValueType::END_OF_FILE)
        })
    });
}

void SequenceZoneDefinitionEntry::ProcessMatch(ZoneDefinition* state, SequenceResult<SimpleParserValue>& result) const
{
    state->m_assets.emplace_back(
        result.NextCapture(CAPTURE_TYPE_NAME).IdentifierValue(),
        result.NextCapture(CAPTURE_ASSET_NAME).IdentifierValue(),
        result.NextTag() == TAG_REFERENCE);
}
