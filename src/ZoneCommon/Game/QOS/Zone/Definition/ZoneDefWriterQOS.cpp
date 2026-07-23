#include "ZoneDefWriterQOS.h"

#include "Game/QOS/QOS.h"

using namespace QOS;

void ZoneDefWriter::WriteMetaData(ZoneDefinitionOutputStream& stream, const Zone& zone) const
{
    (void)stream;
    (void)zone;
}

void ZoneDefWriter::WriteContent(ZoneDefinitionOutputStream& stream, const Zone& zone, const ZoneDefFilter& filter) const
{
    const auto* game = IGame::GetGameById(zone.m_game_id);

    for (const auto& asset : zone.m_pools)
    {
        if (!filter.ShouldWriteAsset(*asset))
            continue;

        stream.WriteEntry(*game->GetAssetTypeName(asset->m_type), asset->m_name);
    }
}
