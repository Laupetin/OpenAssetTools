#include "ZoneDefWriterT5.h"

#include "Game/T5/GameAssetPoolT5.h"
#include "Game/T5/GameT5.h"

#include <cassert>

using namespace T5;

bool ZoneDefWriter::CanHandleZone(Zone* zone) const
{
    return zone->m_game == &g_GameT5;
}

void ZoneDefWriter::WriteMetaData(ZoneDefinitionOutputStream& stream, const UnlinkerArgs* args, Zone* zone) const {}

void ZoneDefWriter::WriteContent(ZoneDefinitionOutputStream& stream, const UnlinkerArgs* args, Zone* zone) const
{
    const auto* pools = dynamic_cast<GameAssetPoolT5*>(zone->m_pools.get());

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
