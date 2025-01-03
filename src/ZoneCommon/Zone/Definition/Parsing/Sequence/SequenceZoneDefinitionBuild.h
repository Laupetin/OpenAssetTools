#pragma once

#include "Zone/Definition/Parsing/ZoneDefinitionParser.h"

class SequenceZoneDefinitionBuild final : public ZoneDefinitionParser::sequence_t
{
    static constexpr auto CAPTURE_BUILD_TARGET_NAME = 1;

protected:
    void ProcessMatch(ZoneDefinitionParserState* state, SequenceResult<ZoneDefinitionParserValue>& result) const override;

public:
    SequenceZoneDefinitionBuild();
};
