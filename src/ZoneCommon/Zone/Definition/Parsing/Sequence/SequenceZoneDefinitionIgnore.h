#pragma once

#include "Zone/Definition/Parsing/ZoneDefinitionParser.h"

class SequenceZoneDefinitionIgnore final : public ZoneDefinitionParser::sequence_t
{
    static constexpr auto CAPTURE_IGNORE_NAME = 1;

protected:
    void ProcessMatch(ZoneDefinitionParserState* state, SequenceResult<ZoneDefinitionParserValue>& result) const override;

public:
    SequenceZoneDefinitionIgnore();
};
