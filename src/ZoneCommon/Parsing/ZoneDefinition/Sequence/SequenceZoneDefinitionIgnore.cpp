#include "SequenceZoneDefinitionIgnore.h"

#include "Parsing/ZoneDefinition/Matcher/ZoneDefinitionMatcherFactory.h"

SequenceZoneDefinitionIgnore::SequenceZoneDefinitionIgnore()
{
    const ZoneDefinitionMatcherFactory create(this);

    AddMatchers({
        create.Keyword("ignore"),
        create.Char(','),
        create.Field().Capture(CAPTURE_IGNORE_NAME),
    });
}

void SequenceZoneDefinitionIgnore::ProcessMatch(ZoneDefinitionParserState* state, SequenceResult<ZoneDefinitionParserValue>& result) const
{
    state->m_definition->m_ignores.emplace_back(result.NextCapture(CAPTURE_IGNORE_NAME).FieldValue());
}
