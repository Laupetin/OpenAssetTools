#include "ZoneDefWriterIW4.h"

#include <cassert>

#include "Game/IW4/GameIW4.h"
#include "Game/IW4/GameAssetPoolIW4.h"

using namespace IW4;

bool ZoneDefWriter::CanHandleZone(Zone* zone) const
{
    return zone->m_game == &g_GameIW4;
}

void ZoneDefWriter::WriteMetaData(ZoneDefinitionOutputStream& stream, const UnlinkerArgs* args, Zone* zone) const
{
}

void ZoneDefWriter::WriteContent(ZoneDefinitionOutputStream& stream, const UnlinkerArgs* args, Zone* zone) const
{
    const auto* pools = dynamic_cast<GameAssetPoolIW4*>(zone->m_pools.get());

    assert(pools);
    if (!pools)
        return;

    // Localized strings are all collected in one string file. So only add this to the zone file.
    if (!pools->m_localize->m_asset_lookup.empty())
    {
        stream.WriteEntry(pools->GetAssetTypeName(ASSET_TYPE_LOCALIZE_ENTRY), zone->m_name);
    }

    for (const auto& asset : *pools)
    {
        switch (asset->m_type)
        {
        case ASSET_TYPE_LOCALIZE_ENTRY:
            break;

        default:
            stream.WriteEntry(pools->GetAssetTypeName(asset->m_type), asset->m_name);
            break;
        }
    }
}
