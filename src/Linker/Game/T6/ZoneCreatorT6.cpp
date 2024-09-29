#include "ZoneCreatorT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/T6.h"
#include "ObjLoading.h"
#include "Utils/StringUtils.h"

#include <iostream>

using namespace T6;

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
    zone->m_pools = std::make_unique<GameAssetPoolT6>(zone, zone->m_priority);

    for (auto assetType = 0; assetType < ASSET_TYPE_COUNT; assetType++)
        zone->m_pools->InitPoolDynamic(assetType);
}

void ZoneCreator::HandleMetadata(Zone* zone, const ZoneCreationContext& context) const
{
    std::vector<KeyValuePair> kvpList;

    for (const auto& metaData : context.m_definition->m_properties.m_properties)
    {
        if (metaData.first.rfind("level.", 0) == 0)
        {
            const std::string strValue = metaData.first.substr(std::char_traits<char>::length("level."));
            if (strValue.empty())
                continue;

            int keyHash;
            if (strValue[0] == '@')
            {
                char* endPtr;
                keyHash = strtol(&strValue[1], &endPtr, 16);

                if (endPtr != &strValue[strValue.size()])
                {
                    std::cout << "Could not parse metadata key \"" << metaData.first << "\" as hash\n";
                    continue;
                }
            }
            else
            {
                keyHash = Common::Com_HashKey(strValue.c_str(), 64);
            }

            KeyValuePair kvp{keyHash, Common::Com_HashKey(zone->m_name.c_str(), 64), zone->GetMemory()->Dup(metaData.second.c_str())};
            kvpList.push_back(kvp);
        }
    }

    if (!kvpList.empty())
    {
        auto* kvps = zone->GetMemory()->Create<KeyValuePairs>();
        kvps->name = zone->GetMemory()->Dup(zone->m_name.c_str());
        kvps->numVariables = kvpList.size();
        kvps->keyValuePairs = zone->GetMemory()->Alloc<KeyValuePair>(kvpList.size());

        for (auto i = 0u; i < kvpList.size(); i++)
            kvps->keyValuePairs[i] = kvpList[i];

        zone->m_pools->AddAsset(std::make_unique<XAssetInfo<KeyValuePairs>>(ASSET_TYPE_KEYVALUEPAIRS, zone->m_name, kvps));
    }
}

GameId ZoneCreator::GetGameId() const
{
    return GameId::T6;
}

std::unique_ptr<Zone> ZoneCreator::CreateZoneForDefinition(ZoneCreationContext& context) const
{
    auto zone = std::make_unique<Zone>(context.m_definition->m_name, 0, &g_GameT6);
    CreateZoneAssetPools(zone.get());

    for (const auto& assetEntry : context.m_definition->m_assets)
    {
        if (!assetEntry.m_is_reference)
            continue;

        context.m_ignored_assets.m_entries.emplace_back(assetEntry.m_asset_type, assetEntry.m_asset_name, assetEntry.m_is_reference);
    }

    const auto assetLoadingContext = std::make_unique<AssetLoadingContext>(*zone, *context.m_asset_search_path, CreateGdtList(context));
    ApplyIgnoredAssets(context, *assetLoadingContext);

    HandleMetadata(zone.get(), context);

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
