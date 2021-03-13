#include "ZoneCreatorIW4.h"

#include "Game/IW4/GameIW4.h"
#include "Game/IW4/GameAssetPoolIW4.h"

using namespace IW4;

ZoneCreator::ZoneCreator()
{
    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
    {
        AddAssetTypeName(assetType, GameAssetPoolIW4::AssetTypeNameByType(assetType));
    }
}

void ZoneCreator::AddAssetTypeName(asset_type_t assetType, std::string name)
{
    m_asset_types_by_name.emplace(std::make_pair(std::move(name), assetType));
}

void ZoneCreator::CreateZoneAssetPools(Zone* zone) const
{
    zone->m_pools = std::make_unique<GameAssetPoolIW4>(zone, zone->m_priority);

    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        zone->m_pools->InitPoolDynamic(assetType);
}

bool ZoneCreator::SupportsGame(const std::string& gameName) const
{
    return gameName == g_GameIW4.GetShortName();
}

std::unique_ptr<Zone> ZoneCreator::CreateZoneForDefinition(ZoneCreationContext& context) const
{
    auto zone = std::make_unique<Zone>(context.m_zone_name, 0, &g_GameIW4);
    zone->m_pools = std::make_unique<GameAssetPoolIW4>(zone.get(), zone->m_priority);

    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        zone->m_pools->InitPoolDynamic(assetType);

    return zone;
}
