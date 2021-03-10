#include "SequenceZoneDefinitionIgnore.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceZoneDefinitionIgnore::SequenceZoneDefinitionIgnore()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("ignore"),
        create.Char(','),
        create.Identifier().Capture(CAPTURE_IGNORE_NAME),
        create.Or({
            create.Type(SimpleParserValueType::NEW_LINE),
            create.Type(SimpleParserValueType::END_OF_FILE)
        })
    });
}

void SequenceZoneDefinitionIgnore::ProcessMatch(ZoneDefinition* state, SequenceResult<SimpleParserValue>& result) const
{
    state->m_ignores.emplace_back(result.NextCapture(CAPTURE_IGNORE_NAME).IdentifierValue());
}
