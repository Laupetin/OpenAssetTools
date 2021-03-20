#pragma once

#include "Parsing/ZoneDefinition/ZoneDefinitionParser.h"

class SequenceZoneDefinitionMetaData final : public ZoneDefinitionParser::sequence_t
{
    static constexpr auto CAPTURE_KEY = 1;
    static constexpr auto CAPTURE_VALUE = 2;

protected:
    void ProcessMatch(ZoneDefinition* state, SequenceResult<ZoneDefinitionParserValue>& result) const override;

public:
    SequenceZoneDefinitionMetaData();
};
