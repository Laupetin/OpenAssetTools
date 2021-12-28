#include "ZoneCreatorT5.h"

#include <iostream>

#include "ObjLoading.h"
#include "AssetLoading/AssetLoadingContext.h"
#include "Game/T5/GameT5.h"
#include "Game/T5/GameAssetPoolT5.h"

using namespace T5;

ZoneCreator::ZoneCreator()
{
    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
    {
        AddAssetTypeName(assetType, GameAssetPoolT5::AssetTypeNameByType(assetType));
    }
}

void ZoneCreator::AddAssetTypeName(asset_type_t assetType, std::string name)
{
    m_asset_types_by_name.emplace(std::make_pair(std::move(name), assetType));
}

std::vector<Gdt*> ZoneCreator::CreateGdtList(ZoneCreationContext& context)
{
    std::vector<Gdt*> gdtList;
    gdtList.reserve(context.m_gdt_files.size());
    for (const auto& gdt : context.m_gdt_files)
        gdtList.push_back(gdt.get());

    return gdtList;
}

bool ZoneCreator::CreateIgnoredAssetMap(ZoneCreationContext& context, std::unordered_map<std::string, asset_type_t>& ignoredAssetMap) const
{
    for (const auto& ignoreEntry : context.m_ignored_assets)
    {
        const auto foundAssetTypeEntry = m_asset_types_by_name.find(ignoreEntry.m_type);
        if (foundAssetTypeEntry == m_asset_types_by_name.end())
        {
            std::cout << "Unknown asset type \"" << ignoreEntry.m_type << "\" for ignore \"" << ignoreEntry.m_name << "\"" << std::endl;
            return false;
        }

        ignoredAssetMap[ignoreEntry.m_name] = foundAssetTypeEntry->second;
    }

    return true;
}

void ZoneCreator::CreateZoneAssetPools(Zone* zone) const
{
    zone->m_pools = std::make_unique<GameAssetPoolT5>(zone, zone->m_priority);

    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        zone->m_pools->InitPoolDynamic(assetType);
}

bool ZoneCreator::SupportsGame(const std::string& gameName) const
{
    return gameName == g_GameT5.GetShortName();
}

std::unique_ptr<Zone> ZoneCreator::CreateZoneForDefinition(ZoneCreationContext& context) const
{
    auto zone = std::make_unique<Zone>(context.m_zone_name, 0, &g_GameT5);
    CreateZoneAssetPools(zone.get());

    for (const auto& assetEntry : context.m_definition->m_assets)
    {
        if (!assetEntry.m_is_reference)
            continue;

        context.m_ignored_assets.emplace_back(assetEntry.m_asset_type, assetEntry.m_asset_name);
    }

    const auto assetLoadingContext = std::make_unique<AssetLoadingContext>(zone.get(), context.m_asset_search_path, CreateGdtList(context));
    if (!CreateIgnoredAssetMap(context, assetLoadingContext->m_ignored_asset_map))
        return nullptr;

    for (const auto& assetEntry : context.m_definition->m_assets)
    {
        const auto foundAssetTypeEntry = m_asset_types_by_name.find(assetEntry.m_asset_type);
        if (foundAssetTypeEntry == m_asset_types_by_name.end())
        {
            std::cout << "Unknown asset type \"" << assetEntry.m_asset_type << "\"" << std::endl;
            return nullptr;
        }

        if (!ObjLoading::LoadAssetForZone(assetLoadingContext.get(), foundAssetTypeEntry->second, assetEntry.m_asset_name))
            return nullptr;
    }

    ObjLoading::FinalizeAssetsForZone(assetLoadingContext.get());

    return zone;
}
