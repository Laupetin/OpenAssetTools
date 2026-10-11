#include "ZoneDefWriterT5.h"

#include "Game/T5/AssetMarkerT5.h"

#include <string_view>

using namespace T5;

namespace
{
    bool IsInternalImpactTableName(const std::string_view name)
    {
        return name.empty() || name == "ImpactFx";
    }

    std::string_view GetImpactTableSourceName(const std::string_view zoneName)
    {
        if (zoneName == "common")
            return "void";

        if (zoneName == "common_mp")
            return "mp_maps";

        if (zoneName == "common_zombie")
            return "zombie";

        return zoneName;
    }
} // namespace

void ZoneDefWriter::WriteMetaData(ZoneDefinitionOutputStream& stream, const Zone& zone) const {}

void ZoneDefWriter::WriteContent(ZoneDefinitionOutputStream& stream, const Zone& zone, const ZoneDefFilter& filter) const
{
    const auto* game = IGame::GetGameById(zone.m_game_id);

    // Localized strings are all collected in one string file. So only add this to the zone file.
    auto localizePoolAssets = zone.m_pools.PoolAssets<AssetLocalize>();
    if (localizePoolAssets.begin() != localizePoolAssets.end())
        stream.WriteEntry(*game->GetAssetTypeName(ASSET_TYPE_LOCALIZE_ENTRY), zone.m_name);

    for (const auto& asset : zone.m_pools)
    {
        if (!filter.ShouldWriteAsset(*asset))
            continue;

        switch (asset->m_type)
        {
        case ASSET_TYPE_LOCALIZE_ENTRY:
            break;

        case ASSET_TYPE_IMPACT_FX:
        {
            // Retail fastfiles use ImpactFx as the internal name; the load-object path assigns an empty name instead.
            const std::string impactTableName = IsInternalImpactTableName(asset->m_name) ? std::string(GetImpactTableSourceName(zone.m_name)) : asset->m_name;
            stream.WriteEntry(*game->GetAssetTypeName(asset->m_type), impactTableName);
            break;
        }

        default:
            stream.WriteEntry(*game->GetAssetTypeName(asset->m_type), asset->m_name);
            break;
        }
    }
}
