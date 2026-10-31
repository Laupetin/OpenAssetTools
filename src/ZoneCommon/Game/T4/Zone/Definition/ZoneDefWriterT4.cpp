#include "ZoneDefWriterT4.h"

#include "Game/T4/T4.h"

#include <string_view>

using namespace T4;

namespace
{
    bool IsCommonZone(const std::string_view zoneName)
    {
        return zoneName == "common" || zoneName == "common_mp";
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
            // CG_RegisterImpactEffects assigns an empty name to both root and map-specific tables.
            const auto impactTableName = asset->m_name.empty() ? (IsCommonZone(zone.m_name) ? "void" : zone.m_name) : asset->m_name;
            stream.WriteEntry(*game->GetAssetTypeName(asset->m_type), impactTableName);
            break;
        }

        default:
            stream.WriteEntry(*game->GetAssetTypeName(asset->m_type), asset->m_name);
            break;
        }
    }
}
