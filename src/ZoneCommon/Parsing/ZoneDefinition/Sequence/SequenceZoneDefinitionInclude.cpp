#include "SequenceZoneDefinitionInclude.h"

#include "Parsing/ZoneDefinition/Matcher/ZoneDefinitionMatcherFactory.h"

SequenceZoneDefinitionInclude::SequenceZoneDefinitionInclude()
{
    const ZoneDefinitionMatcherFactory create(this);

    AddMatchers({
        create.Keyword("include"),
        create.Char(','),
        create.Field().Capture(CAPTURE_INCLUDE_NAME),
    });
}

void SequenceZoneDefinitionInclude::ProcessMatch(ZoneDefinitionParserState* state, SequenceResult<ZoneDefinitionParserValue>& result) const
{
    state->m_definition->m_includes.emplace_back(result.NextCapture(CAPTURE_INCLUDE_NAME).FieldValue());
}
