#pragma once

#include "Parsing/ZoneDefinition/ZoneDefinitionParser.h"

class SequenceZoneDefinitionAssetList final : public ZoneDefinitionParser::sequence_t
{
    static constexpr auto CAPTURE_ASSET_LIST_NAME = 1;

protected:
    void ProcessMatch(ZoneDefinitionParserState* state, SequenceResult<ZoneDefinitionParserValue>& result) const override;

public:
    SequenceZoneDefinitionAssetList();
};
