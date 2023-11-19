#include "SequenceZoneDefinitionMetaData.h"

#include "Parsing/ZoneDefinition/Matcher/ZoneDefinitionMatcherFactory.h"

SequenceZoneDefinitionMetaData::SequenceZoneDefinitionMetaData()
{
    const ZoneDefinitionMatcherFactory create(this);

    AddMatchers({create.Char('>'), create.Field().Capture(CAPTURE_KEY), create.Char(','), create.Field().Capture(CAPTURE_VALUE)});
}

void SequenceZoneDefinitionMetaData::ProcessMatch(ZoneDefinition* state, SequenceResult<ZoneDefinitionParserValue>& result) const
{
    state->AddMetaData(result.NextCapture(CAPTURE_KEY).FieldValue(), result.NextCapture(CAPTURE_VALUE).FieldValue());
}
