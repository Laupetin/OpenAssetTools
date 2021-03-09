#include "SequenceZoneDefinitionEntry.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceZoneDefinitionEntry::SequenceZoneDefinitionEntry()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Identifier().Capture(CAPTURE_TYPE_NAME),
        create.Char(','),
        create.Optional(create.Char(',').Tag(TAG_REFERENCE)),
        create.Identifier().Capture(CAPTURE_ASSET_NAME)
    });
}

void SequenceZoneDefinitionEntry::ProcessMatch(ZoneDefinition* state, SequenceResult<SimpleParserValue>& result) const
{
    state->m_assets.emplace_back(
        result.NextCapture(CAPTURE_TYPE_NAME).IdentifierValue(),
        result.NextCapture(CAPTURE_ASSET_NAME).IdentifierValue(),
        result.NextTag() == TAG_REFERENCE);
}
