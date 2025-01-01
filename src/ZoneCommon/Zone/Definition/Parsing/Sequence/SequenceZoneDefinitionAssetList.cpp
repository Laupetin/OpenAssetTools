#include "SequenceZoneDefinitionAssetList.h"

#include "Zone/AssetList/AssetListReader.h"
#include "Zone/Definition/Parsing/Matcher/ZoneDefinitionMatcherFactory.h"

SequenceZoneDefinitionAssetList::SequenceZoneDefinitionAssetList()
{
    const ZoneDefinitionMatcherFactory create(this);

    AddMatchers({
        create.Keyword("assetlist").Capture(CAPTURE_ASSET_LIST_KEYWORD),
        create.Char(','),
        create.Field().Capture(CAPTURE_ASSET_LIST_NAME),
    });
}

void SequenceZoneDefinitionAssetList::ProcessMatch(ZoneDefinitionParserState* state, SequenceResult<ZoneDefinitionParserValue>& result) const
{
    if (state->m_definition->m_game == GameId::COUNT)
    {
        const auto& assetListKeywordToken = result.NextCapture(CAPTURE_ASSET_LIST_KEYWORD);
        throw ParsingException(assetListKeywordToken.GetPos(), "Must define game before using assetlist");
    }

    const auto& assetListNameToken = result.NextCapture(CAPTURE_ASSET_LIST_NAME);

    AssetListReader assetListReader(state->m_search_path, state->m_definition->m_game);
    const auto maybeAssetList = assetListReader.ReadAssetList(assetListNameToken.FieldValue());

    if (!maybeAssetList)
        throw ParsingException(assetListNameToken.GetPos(), "Failed to read asset list");

    for (auto& assetListEntry : maybeAssetList->m_entries)
        state->m_definition->m_assets.emplace_back(assetListEntry.m_type, std::move(assetListEntry.m_name), assetListEntry.m_is_reference);
}
