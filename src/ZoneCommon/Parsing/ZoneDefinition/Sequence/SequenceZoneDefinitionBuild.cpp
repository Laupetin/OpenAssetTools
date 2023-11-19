#include "SequenceZoneDefinitionBuild.h"

#include "Parsing/ZoneDefinition/Matcher/ZoneDefinitionMatcherFactory.h"

SequenceZoneDefinitionBuild::SequenceZoneDefinitionBuild()
{
    const ZoneDefinitionMatcherFactory create(this);

    AddMatchers({create.Keyword("build"), create.Char(','), create.Field().Capture(CAPTURE_BUILD_TARGET_NAME)});
}

void SequenceZoneDefinitionBuild::ProcessMatch(ZoneDefinition* state, SequenceResult<ZoneDefinitionParserValue>& result) const
{
    state->m_targets_to_build.emplace_back(result.NextCapture(CAPTURE_BUILD_TARGET_NAME).FieldValue());
}
