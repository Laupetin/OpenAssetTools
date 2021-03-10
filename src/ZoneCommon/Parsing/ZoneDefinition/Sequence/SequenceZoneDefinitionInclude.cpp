#include "SequenceZoneDefinitionInclude.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceZoneDefinitionInclude::SequenceZoneDefinitionInclude()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("include"),
        create.Char(','),
        create.Identifier().Capture(CAPTURE_INCLUDE_NAME),
        create.Or({
            create.Type(SimpleParserValueType::NEW_LINE),
            create.Type(SimpleParserValueType::END_OF_FILE)
        })
    });
}

void SequenceZoneDefinitionInclude::ProcessMatch(ZoneDefinition* state, SequenceResult<SimpleParserValue>& result) const
{
    state->m_includes.emplace_back(result.NextCapture(CAPTURE_INCLUDE_NAME).IdentifierValue());
}
