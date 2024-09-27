#include "ZoneCreatorIW4.h"

#include "Game/IW4/GameAssetPoolIW4.h"
#include "Game/IW4/GameIW4.h"
#include "ObjLoading.h"
#include "Utils/StringUtils.h"

#include <iostream>

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

std::vector<Gdt*> ZoneCreator::CreateGdtList(const ZoneCreationContext& context)
{
    std::vector<Gdt*> gdtList;
    gdtList.reserve(context.m_gdt_files.size());
    for (const auto& gdt : context.m_gdt_files)
        gdtList.push_back(gdt.get());

    return gdtList;
}

bool ZoneCreator::CreateIgnoredAssetMap(const ZoneCreationContext& context, std::unordered_map<std::string, asset_type_t>& ignoredAssetMap) const
{
    for (const auto& ignoreEntry : context.m_ignored_assets.m_entries)
    {
        const auto foundAssetTypeEntry = m_asset_types_by_name.find(ignoreEntry.m_type);
        if (foundAssetTypeEntry == m_asset_types_by_name.end())
        {
            std::cout << "Unknown asset type \"" << ignoreEntry.m_type << "\" for ignore \"" << ignoreEntry.m_name << "\"\n";
            return false;
        }

        ignoredAssetMap[ignoreEntry.m_name] = foundAssetTypeEntry->second;
    }

    return true;
}

void ZoneCreator::CreateZoneAssetPools(Zone* zone) const
{
    zone->m_pools = std::make_unique<GameAssetPoolIW4>(zone, zone->m_priority);

    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        zone->m_pools->InitPoolDynamic(assetType);
}

bool ZoneCreator::SupportsGame(const std::string& gameName) const
{
    auto shortName = g_GameIW4.GetShortName();
    utils::MakeStringLowerCase(shortName);

    return gameName == shortName;
}

std::unique_ptr<Zone> ZoneCreator::CreateZoneForDefinition(ZoneCreationContext& context) const
{
    auto zone = std::make_unique<Zone>(context.m_definition->m_name, 0, &g_GameIW4);
    CreateZoneAssetPools(zone.get());

    for (const auto& assetEntry : context.m_definition->m_assets)
    {
        if (!assetEntry.m_is_reference)
            continue;

        context.m_ignored_assets.m_entries.emplace_back(assetEntry.m_asset_type, assetEntry.m_asset_name, assetEntry.m_is_reference);
    }

    const auto assetLoadingContext = std::make_unique<AssetLoadingContext>(*zone, *context.m_asset_search_path, CreateGdtList(context));
    if (!CreateIgnoredAssetMap(context, assetLoadingContext->m_ignored_asset_map))
        return nullptr;

    for (const auto& assetEntry : context.m_definition->m_assets)
    {
        const auto foundAssetTypeEntry = m_asset_types_by_name.find(assetEntry.m_asset_type);
        if (foundAssetTypeEntry == m_asset_types_by_name.end())
        {
            std::cout << "Unknown asset type \"" << assetEntry.m_asset_type << "\"\n";
            return nullptr;
        }

        if (!ObjLoading::LoadAssetForZone(*assetLoadingContext, foundAssetTypeEntry->second, assetEntry.m_asset_name))
            return nullptr;
    }

    ObjLoading::FinalizeAssetsForZone(*assetLoadingContext);

    return zone;
}
