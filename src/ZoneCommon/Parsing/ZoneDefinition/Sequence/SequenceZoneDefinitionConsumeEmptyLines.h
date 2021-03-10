#pragma once

#include "Parsing/ZoneDefinition/ZoneDefinitionParser.h"

class SequenceZoneDefinitionConsumeEmptyLines final : public ZoneDefinitionParser::sequence_t
{
protected:
    void ProcessMatch(ZoneDefinition* state, SequenceResult<SimpleParserValue>& result) const override;

public:
    SequenceZoneDefinitionConsumeEmptyLines();
};
