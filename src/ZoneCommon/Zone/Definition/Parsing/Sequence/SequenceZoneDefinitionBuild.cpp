#include "SequenceZoneDefinitionBuild.h"

#include "Zone/Definition/Parsing/Matcher/ZoneDefinitionMatcherFactory.h"

SequenceZoneDefinitionBuild::SequenceZoneDefinitionBuild()
{
    const ZoneDefinitionMatcherFactory create(this);

    AddMatchers({
        create.Keyword("build"),
        create.Char(','),
        create.Field().Capture(CAPTURE_BUILD_TARGET_NAME),
    });
}

void SequenceZoneDefinitionBuild::ProcessMatch(ZoneDefinitionParserState* state, SequenceResult<ZoneDefinitionParserValue>& result) const
{
    state->m_definition->m_targets_to_build.emplace_back(result.NextCapture(CAPTURE_BUILD_TARGET_NAME).FieldValue());
}
