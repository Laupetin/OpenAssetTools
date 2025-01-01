#pragma once

#include "Zone/Definition/Parsing/ZoneDefinitionParser.h"

class SequenceZoneDefinitionAssetList final : public ZoneDefinitionParser::sequence_t
{
    static constexpr auto CAPTURE_ASSET_LIST_KEYWORD = 1;
    static constexpr auto CAPTURE_ASSET_LIST_NAME = 2;

protected:
    void ProcessMatch(ZoneDefinitionParserState* state, SequenceResult<ZoneDefinitionParserValue>& result) const override;

public:
    SequenceZoneDefinitionAssetList();
};
