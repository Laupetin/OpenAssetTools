#include "ZoneCreatorIW5.h"

#include "Game/IW5/GameAssetPoolIW5.h"
#include "Game/IW5/GameIW5.h"
#include "ObjLoading.h"
#include "Utils/StringUtils.h"

using namespace IW5;

std::vector<Gdt*> ZoneCreator::CreateGdtList(const ZoneCreationContext& context)
{
    std::vector<Gdt*> gdtList;
    gdtList.reserve(context.m_gdt_files.size());
    for (const auto& gdt : context.m_gdt_files)
        gdtList.push_back(gdt.get());

    return gdtList;
}

void ZoneCreator::ApplyIgnoredAssets(const ZoneCreationContext& creationContext, AssetLoadingContext& loadingContext)
{
    for (const auto& ignoreEntry : creationContext.m_ignored_assets.m_entries)
        loadingContext.m_ignored_asset_map[ignoreEntry.m_name] = ignoreEntry.m_type;
}

void ZoneCreator::CreateZoneAssetPools(Zone* zone) const
{
    zone->m_pools = std::make_unique<GameAssetPoolIW5>(zone, zone->m_priority);

    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        zone->m_pools->InitPoolDynamic(assetType);
}

GameId ZoneCreator::GetGameId() const
{
    return GameId::IW5;
}

std::unique_ptr<Zone> ZoneCreator::CreateZoneForDefinition(ZoneCreationContext& context) const
{
    auto zone = std::make_unique<Zone>(context.m_definition->m_name, 0, &g_GameIW5);
    CreateZoneAssetPools(zone.get());

    for (const auto& assetEntry : context.m_definition->m_assets)
    {
        if (!assetEntry.m_is_reference)
            continue;

        context.m_ignored_assets.m_entries.emplace_back(assetEntry.m_asset_type, assetEntry.m_asset_name, assetEntry.m_is_reference);
    }

    const auto assetLoadingContext = std::make_unique<AssetLoadingContext>(*zone, *context.m_asset_search_path, CreateGdtList(context));
    ApplyIgnoredAssets(context, *assetLoadingContext);

    for (const auto& assetEntry : context.m_definition->m_assets)
    {
        if (!ObjLoading::LoadAssetForZone(*assetLoadingContext, assetEntry.m_asset_type, assetEntry.m_asset_name))
            return nullptr;
    }

    ObjLoading::FinalizeAssetsForZone(*assetLoadingContext);

    return zone;
}

asset_type_t ZoneCreator::GetImageAssetType() const
{
    return ASSET_TYPE_IMAGE;
}
