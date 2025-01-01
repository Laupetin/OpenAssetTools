#include "SequenceZoneDefinitionInclude.h"

#include "Zone/Definition/Parsing/Matcher/ZoneDefinitionMatcherFactory.h"

#include <format>

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
    const auto& inclusionNameToken = result.NextCapture(CAPTURE_INCLUDE_NAME);
    const auto inclusionName = inclusionNameToken.FieldValue();
    const auto existingInclusion = state->m_inclusions.find(inclusionName);
    if (existingInclusion != state->m_inclusions.end())
        throw ParsingException(inclusionNameToken.GetPos(), "Zone definition has already been included");

    const auto zoneDefinitionFileNameToInclude = std::format("{}.zone", inclusionName);

    if (!state->m_underlying_stream.IncludeFile(zoneDefinitionFileNameToInclude))
        throw ParsingException(inclusionNameToken.GetPos(), "Could not find zone definition with this filename");

    state->m_inclusions.emplace(inclusionName);
}
