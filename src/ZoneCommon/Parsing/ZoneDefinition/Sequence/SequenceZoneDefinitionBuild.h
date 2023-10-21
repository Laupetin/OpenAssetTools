#pragma once

#include "Parsing/ZoneDefinition/ZoneDefinitionParser.h"

class SequenceZoneDefinitionBuild final : public ZoneDefinitionParser::sequence_t
{
    static constexpr auto CAPTURE_BUILD_TARGET_NAME = 1;

protected:
    void ProcessMatch(ZoneDefinition* state, SequenceResult<ZoneDefinitionParserValue>& result) const override;

public:
    SequenceZoneDefinitionBuild();
};
