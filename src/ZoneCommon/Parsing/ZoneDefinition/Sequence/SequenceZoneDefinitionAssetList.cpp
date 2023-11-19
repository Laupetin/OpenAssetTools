#include "SequenceZoneDefinitionAssetList.h"

#include "Parsing/ZoneDefinition/Matcher/ZoneDefinitionMatcherFactory.h"

SequenceZoneDefinitionAssetList::SequenceZoneDefinitionAssetList()
{
    const ZoneDefinitionMatcherFactory create(this);

    AddMatchers({create.Keyword("assetlist"), create.Char(','), create.Field().Capture(CAPTURE_ASSET_LIST_NAME)});
}

void SequenceZoneDefinitionAssetList::ProcessMatch(ZoneDefinition* state, SequenceResult<ZoneDefinitionParserValue>& result) const
{
    state->m_asset_lists.emplace_back(result.NextCapture(CAPTURE_ASSET_LIST_NAME).FieldValue());
}
