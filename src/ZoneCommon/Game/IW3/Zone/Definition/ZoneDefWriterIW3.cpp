#include "ZoneDefWriterIW3.h"

#include "Game/IW3/IW3.h"

using namespace IW3;

void ZoneDefWriter::WriteMetaData(ZoneDefinitionOutputStream& stream, const Zone& zone) const {}

void ZoneDefWriter::WriteContent(ZoneDefinitionOutputStream& stream, const Zone& zone) const
{
    const auto* game = IGame::GetGameById(zone.m_game_id);

    // Localized strings are all collected in one string file. So only add this to the zone file.
    auto localizePoolAssets = zone.m_pools.PoolAssets<AssetLocalize>();
    if (localizePoolAssets.begin() != localizePoolAssets.end())
        stream.WriteEntry(*game->GetAssetTypeName(ASSET_TYPE_LOCALIZE_ENTRY), zone.m_name);

    for (const auto& asset : zone.m_pools)
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
