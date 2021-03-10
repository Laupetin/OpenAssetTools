#include "SequenceZoneDefinitionMetaData.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceZoneDefinitionMetaData::SequenceZoneDefinitionMetaData()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Char('>'),
        create.Identifier().Capture(CAPTURE_KEY),
        create.Char(','),
        create.Identifier().Capture(CAPTURE_VALUE),
        create.Type(SimpleParserValueType::NEW_LINE)
    });
}

void SequenceZoneDefinitionMetaData::ProcessMatch(ZoneDefinition* state, SequenceResult<SimpleParserValue>& result) const
{
    state->m_metadata[result.NextCapture(CAPTURE_KEY).IdentifierValue()] = result.NextCapture(CAPTURE_VALUE).IdentifierValue();
}
