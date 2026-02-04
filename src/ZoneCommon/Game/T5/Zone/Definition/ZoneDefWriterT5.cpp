#include "ZoneDefWriterT5.h"

#include "Game/T5/GameAssetPoolT5.h"

#include <cassert>

using namespace T5;

void ZoneDefWriter::WriteMetaData(ZoneDefinitionOutputStream& stream, const Zone& zone) const {}

void ZoneDefWriter::WriteContent(ZoneDefinitionOutputStream& stream, const Zone& zone) const
{
    const auto* game = IGame::GetGameById(zone.m_game_id);
    const auto* pools = dynamic_cast<GameAssetPoolT5*>(zone.m_pools.get());

    assert(pools);
    if (!pools)
        return;

    // Localized strings are all collected in one string file. So only add this to the zone file.
    if (!pools->m_localize->m_asset_lookup.empty())
        stream.WriteEntry(*game->GetAssetTypeName(ASSET_TYPE_LOCALIZE_ENTRY), zone.m_name);

    for (const auto& asset : *pools)
    {
        switch (asset->m_type)
        {
        case ASSET_TYPE_LOCALIZE_ENTRY:
            break;

        default:
            stream.WriteEntry(*game->GetAssetTypeName(asset->m_type), asset->m_name);
            break;
        }
    }
}
