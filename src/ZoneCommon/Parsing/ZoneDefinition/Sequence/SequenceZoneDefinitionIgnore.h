#pragma once

#include "Parsing/ZoneDefinition/ZoneDefinitionParser.h"

class SequenceZoneDefinitionIgnore final : public ZoneDefinitionParser::sequence_t
{
    static constexpr auto CAPTURE_IGNORE_NAME = 1;

protected:
    void ProcessMatch(ZoneDefinition* state, SequenceResult<ZoneDefinitionParserValue>& result) const override;

public:
    SequenceZoneDefinitionIgnore();
};
