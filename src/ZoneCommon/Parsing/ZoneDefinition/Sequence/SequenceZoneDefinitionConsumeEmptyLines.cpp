#include "SequenceZoneDefinitionConsumeEmptyLines.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceZoneDefinitionConsumeEmptyLines::SequenceZoneDefinitionConsumeEmptyLines()
{
    const SimpleMatcherFactory create(this);

    AddMatchers(create.Loop(create.Type(SimpleParserValueType::NEW_LINE)));
}

void SequenceZoneDefinitionConsumeEmptyLines::ProcessMatch(ZoneDefinition* state, SequenceResult<SimpleParserValue>& result) const
{
}
