#include "SequenceZoneDefinitionIgnore.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceZoneDefinitionIgnore::SequenceZoneDefinitionIgnore()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("ignore"),
        create.Char(','),
        create.Identifier().Capture(CAPTURE_IGNORE_NAME)
        });
}

void SequenceZoneDefinitionIgnore::ProcessMatch(ZoneDefinition* state, SequenceResult<SimpleParserValue>& result) const
{
    state->m_ignores.emplace_back(result.NextCapture(CAPTURE_IGNORE_NAME).IdentifierValue());
}
