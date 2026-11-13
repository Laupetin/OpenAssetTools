#include "ZoneDefWriter.h"

#include "Game/IW3/Zone/Definition/ZoneDefWriterIW3.h"
#include "Game/IW4/Zone/Definition/ZoneDefWriterIW4.h"
#include "Game/IW5/Zone/Definition/ZoneDefWriterIW5.h"
#include "Game/QOS/Zone/Definition/ZoneDefWriterQOS.h"
#include "Game/T4/Zone/Definition/ZoneDefWriterT4.h"
#include "Game/T5/Zone/Definition/ZoneDefWriterT5.h"
#include "Game/T6/Zone/Definition/ZoneDefWriterT6.h"
#include "Pool/XAssetInfo.h"

#include <cassert>
#include <format>

namespace
{
    constexpr auto META_DATA_KEY_GAME = "game";
    constexpr auto META_DATA_KEY_GDT = "gdt";
} // namespace

ZoneDefFilter ZoneDefFilter::AllEntries()
{
    return ZoneDefFilter();
}

ZoneDefFilter ZoneDefFilter::WithOnlyRootAssets(const Zone& zone)
{
    ZoneDefFilter result;

    for (const auto& asset : zone.m_pools)
    {
        for (const auto* dependency : asset->m_dependencies)
        {
            if (dependency != nullptr && dependency->m_zone == &zone)
                result.m_non_root_asset_lookup.emplace(dependency);
        }
    }

    return std::move(result);
}

void ZoneDefFilter::RestrictToAssetTypes(const Zone& zone, std::vector<bool> assetTypes)
{
    m_asset_types = std::move(assetTypes);
    m_reference_asset_lookup.clear();

    for (const auto& asset : zone.m_pools)
    {
        if (!ShouldWriteAssetType(asset->m_type))
            continue;

        for (const auto* dependency : asset->m_dependencies)
        {
            if (dependency != nullptr && dependency->m_zone == &zone && !ShouldWriteAssetType(dependency->m_type))
                m_reference_asset_lookup.emplace(dependency);
        }
    }
}

bool ZoneDefFilter::ShouldWriteAssetType(const asset_type_t assetType) const
{
    if (assetType < 0 || static_cast<size_t>(assetType) >= m_asset_types.size())
        return true;

    return m_asset_types[assetType];
}

bool ZoneDefFilter::ShouldWriteAsset(const XAssetInfoGeneric& asset) const
{
    if (m_reference_asset_lookup.contains(&asset))
        return true;

    if (!ShouldWriteAssetType(asset.m_type))
        return false;

    return m_non_root_asset_lookup.empty() || !m_non_root_asset_lookup.contains(&asset);
}

std::string ZoneDefFilter::EntryName(const XAssetInfoGeneric& asset) const
{
    if (!asset.IsReference() && m_reference_asset_lookup.contains(&asset))
        return std::format(",{}", asset.m_name);

    return asset.m_name;
}

const IZoneDefWriter* IZoneDefWriter::GetZoneDefWriterForGame(GameId game)
{
    static const IZoneDefWriter* zoneDefWriters[]{
        new IW3::ZoneDefWriter(),
        new IW4::ZoneDefWriter(),
        new IW5::ZoneDefWriter(),
        new QOS::ZoneDefWriter(),
        new T4::ZoneDefWriter(),
        new T5::ZoneDefWriter(),
        new T6::ZoneDefWriter(),
    };
    static_assert(std::extent_v<decltype(zoneDefWriters)> == static_cast<unsigned>(GameId::COUNT));

    assert(static_cast<unsigned>(game) < static_cast<unsigned>(GameId::COUNT));
    const auto* result = zoneDefWriters[static_cast<unsigned>(game)];
    assert(result);

    return result;
}

void AbstractZoneDefWriter::WriteZoneDef(std::ostream& stream, const Zone& zone, const bool useGdt, const bool minimalZone, std::vector<bool> assetTypes) const
{
    ZoneDefinitionOutputStream out(stream);
    const auto* game = IGame::GetGameById(zone.m_game_id);

    out.WriteComment(game->GetFullName());
    out.WriteMetaData(META_DATA_KEY_GAME, game->GetShortName());
    out.EmptyLine();

    if (useGdt)
    {
        out.WriteComment("Load asset gdt files");
        out.WriteMetaData(META_DATA_KEY_GDT, zone.m_name);
        out.EmptyLine();
    }

    WriteMetaData(out, zone);

    auto filter = minimalZone ? ZoneDefFilter::WithOnlyRootAssets(zone) : ZoneDefFilter::AllEntries();
    filter.RestrictToAssetTypes(zone, std::move(assetTypes));
    WriteContent(out, zone, filter);
}
