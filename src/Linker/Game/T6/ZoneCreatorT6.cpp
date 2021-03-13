#include "ZoneCreatorT6.h"

#include <iostream>

#include "ObjLoading.h"
#include "Game/T6/T6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/GameAssetPoolT6.h"

using namespace T6;

ZoneCreator::ZoneCreator()
{
    for(auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
    {
        AddAssetTypeName(assetType, GameAssetPoolT6::AssetTypeNameByType(assetType));
    }
}

void ZoneCreator::AddAssetTypeName(asset_type_t assetType, std::string name)
{
    m_asset_types_by_name.emplace(std::make_pair(std::move(name), assetType));
}

void ZoneCreator::CreateZoneAssetPools(Zone* zone) const
{
    zone->m_pools = std::make_unique<GameAssetPoolT6>(zone, zone->m_priority);

    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        zone->m_pools->InitPoolDynamic(assetType);
}

bool ZoneCreator::SupportsGame(const std::string& gameName) const
{
    return gameName == g_GameT6.GetShortName();
}

std::unique_ptr<Zone> ZoneCreator::CreateZoneForDefinition(ZoneCreationContext& context) const
{
    auto zone = std::make_unique<Zone>(context.m_zone_name, 0, &g_GameT6);
    CreateZoneAssetPools(zone.get());

    std::vector<Gdt*> gdtList;
    gdtList.reserve(context.m_gdt_files.size());
    for (const auto& gdt : context.m_gdt_files)
        gdtList.push_back(gdt.get());
    const auto assetLoadingContext = std::make_unique<AssetLoadingContext>(zone.get(), context.m_asset_search_path, std::move(gdtList));

    for(const auto& assetEntry : context.m_definition->m_assets)
    {
        if (assetEntry.m_is_reference)
            continue;

        const auto foundAssetTypeEntry = m_asset_types_by_name.find(assetEntry.m_asset_type);
        if(foundAssetTypeEntry == m_asset_types_by_name.end())
        {
            std::cout << "Unknown asset type \"" << assetEntry.m_asset_type << "\"" << std::endl;
            return nullptr;
        }

        if (!ObjLoading::LoadAssetForZone(assetLoadingContext.get(), foundAssetTypeEntry->second, assetEntry.m_asset_name))
            return nullptr;
    }

    return zone;
}
